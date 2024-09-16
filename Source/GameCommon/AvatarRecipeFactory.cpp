#include "AvatarRecipeFactory.h"
#include "Frameworks/ServiceManager.h"
#include "AvatarFactoryCommands.h"
#include "AvatarRecipeQueries.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/QueryDispatcher.h"
#include "Platforms/PlatformLayer.h"

using namespace Enigma::GameCommon;
using namespace Enigma::Frameworks;

DEFINE_RTTI(GameCommon, AvatarRecipeFactory, ISystemService);

AvatarRecipeFactory::AvatarRecipeFactory(ServiceManager* srv_manager) : ISystemService(srv_manager)
{
    m_recipeCreators.clear();

    m_registerRecipeFactory = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { registerRecipeFactory(c); });
    CommandBus::subscribe(typeid(RegisterAvatarRecipeFactory), m_registerRecipeFactory);
    m_unregisterRecipeFactory = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { unregisterRecipeFactory(c); });
    CommandBus::subscribe(typeid(UnregisterAvatarRecipeFactory), m_unregisterRecipeFactory);

    m_requestRecipeCreation = std::make_shared<QuerySubscriber>([=](const IQueryPtr& q) { requestRecipeCreation(q); });
    QueryDispatcher::subscribe(typeid(RequestAvatarRecipeCreation), m_requestRecipeCreation);
}

AvatarRecipeFactory::~AvatarRecipeFactory()
{
    CommandBus::unsubscribe(typeid(RegisterAvatarRecipeFactory), m_registerRecipeFactory);
    m_registerRecipeFactory = nullptr;
    CommandBus::unsubscribe(typeid(UnregisterAvatarRecipeFactory), m_unregisterRecipeFactory);
    m_unregisterRecipeFactory = nullptr;

    QueryDispatcher::unsubscribe(typeid(RequestAvatarRecipeCreation), m_requestRecipeCreation);
    m_requestRecipeCreation = nullptr;

    m_recipeCreators.clear();
}

void AvatarRecipeFactory::registerRecipeFactory(const std::string& rtti, RecipeCreator creator)
{
    if (m_recipeCreators.find(rtti) != m_recipeCreators.end())
    {
        Platforms::Debug::Printf("Recipe factory of %s already exists\n", rtti.c_str());
        return;
    }
    m_recipeCreators[rtti] = creator;
}

void AvatarRecipeFactory::unregisterRecipeFactory(const std::string& rtti)
{
    if (m_recipeCreators.find(rtti) == m_recipeCreators.end())
    {
        Platforms::Debug::Printf("Recipe factory of %s doesn't exist\n", rtti.c_str());
        return;
    }
    m_recipeCreators.erase(rtti);
}

std::shared_ptr<AvatarRecipe> AvatarRecipeFactory::createRecipe(const std::string& rtti) const
{
    const auto it = m_recipeCreators.find(rtti);
    if (it == m_recipeCreators.end())
    {
        Platforms::Debug::Printf("Recipe factory of %s doesn't exist\n", rtti.c_str());
        return nullptr;
    }
    return it->second();
}

void AvatarRecipeFactory::registerRecipeFactory(const ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::static_pointer_cast<RegisterAvatarRecipeFactory>(c);
    if (!cmd) return;
    registerRecipeFactory(cmd->rtti(), cmd->creator());
}

void AvatarRecipeFactory::unregisterRecipeFactory(const ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::static_pointer_cast<UnregisterAvatarRecipeFactory>(c);
    if (!cmd) return;
    unregisterRecipeFactory(cmd->rtti());
}

void AvatarRecipeFactory::requestRecipeCreation(const IQueryPtr& q)
{
    if (!q) return;
    const auto query = std::static_pointer_cast<RequestAvatarRecipeCreation>(q);
    if (!query) return;
    const auto recipe = createRecipe(query->rtti());
    query->setResult(recipe);
}
