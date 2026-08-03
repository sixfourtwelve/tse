#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#define NON_COPYABLE(Class)                                                                                            \
    Class(const Class&) = delete;                                                                                      \
    Class& operator=(const Class&) = delete

constexpr std::size_t kMaxEntityCount = 1'000;

using EntityId = std::uint32_t;

enum class EEntityType : std::uint8_t
{
    Player = 0,
    Enemy
};

class CEntity
{
public:
    explicit CEntity(EEntityType entityType) noexcept
        : mType(entityType)
    {
    }

    virtual ~CEntity() = default;

    virtual void Think() {}
    virtual void Draw() = 0;

    [[nodiscard]] bool IsKindOf(EEntityType type) const noexcept { return mType == type; }

    bool mIsActive = false;

private:
    EEntityType mType;
};

class CPlayer final : public CEntity
{
public:
    CPlayer() noexcept
        : CEntity(EEntityType::Player)
    {
    }

    void Draw() override
    {
        // ... draw to the screen
    }
};

class CEnemy final : public CEntity
{
public:
    CEnemy(std::string name, int initialHealth)
        : CEntity(EEntityType::Enemy)
        , mEnemyName(std::move(name))
        , mHealth(initialHealth)
    {
    }

    void Draw() override
    {
        // ... draw the enemy
    }

    void Think() override
    {
        // ... it has a brain
    }

    [[nodiscard]] const std::string& GetName() const noexcept { return mEnemyName; }

    [[nodiscard]] int GetHealth() const noexcept { return mHealth; }

private:
    std::string mEnemyName;
    int mHealth;
};

class CObjectManager
{
public:
    CObjectManager()
    {
        mEntities.reserve(kMaxEntityCount);
        mPendingRemovals.reserve(kMaxEntityCount);
    }

    NON_COPYABLE(CObjectManager);

    CObjectManager(CObjectManager&&) noexcept = default;
    CObjectManager& operator=(CObjectManager&&) noexcept = default;

    void Update()
    {
        for (const EntitySlot& slot : mEntities)
            slot.mEntity->Think();

        FlushPendingRemovals();
    }

    [[nodiscard]] bool AddEntity(std::unique_ptr<CEntity> entity, EntityId id)
    {
        if (!entity || mEntities.size() >= kMaxEntityCount)
            return false;

        const bool idAlreadyExists = std::any_of(
            mEntities.cbegin(), mEntities.cend(), [id](const EntitySlot& slot) { return slot.mId == id; });

        if (idAlreadyExists)
            return false;

        mEntities.push_back(EntitySlot{
            .mEntity = std::move(entity),
            .mId = id,
        });
        return true;
    }

    void RemoveEntity(EntityId id)
    {
        if (mPendingRemovals.size() >= kMaxEntityCount)
            return;

        mPendingRemovals.push_back(id);
    }

private:
    struct EntitySlot
    {
        std::unique_ptr<CEntity> mEntity;
        EntityId mId;
    };

    void FlushPendingRemovals()
    {
        for (const EntityId id : mPendingRemovals)
            DoRemove(id);

        mPendingRemovals.clear();
    }

    void DoRemove(EntityId id)
    {
        const auto slot = std::find_if(
            mEntities.begin(), mEntities.end(), [id](const EntitySlot& candidate) { return candidate.mId == id; });

        if (slot == mEntities.end())
            return;

        if (slot != mEntities.end() - 1)
            *slot = std::move(mEntities.back());

        mEntities.pop_back();
    }

    std::vector<EntitySlot> mEntities;
    std::vector<EntityId> mPendingRemovals;
};

int main()
{
    CObjectManager entityManager;

    const bool playerAdded = entityManager.AddEntity(std::make_unique<CPlayer>(), 1);
    const bool enemyAdded = entityManager.AddEntity(std::make_unique<CEnemy>("Goblin", 100), 2);

    if (!playerAdded || !enemyAdded)
        return 1;

    entityManager.Update();
    entityManager.RemoveEntity(2);
    entityManager.Update();

    return 0;
}
