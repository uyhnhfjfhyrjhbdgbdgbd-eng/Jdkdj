#include <Geode/Geode.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>
#include <Geode/utils/web.hpp>
using namespace geode::prelude;

namespace {
    EventListener<web::WebTask> g_listener;

    std::string clean(std::string s) {
        for (auto& c : s) {
            if (c == '<' || c == '>') c = ' ';
        }
        return s;
    }

    void fetchLevel(int id) {
        std::string url = Mod::get()->getSettingValue<std::string>("api-url");
        auto pos = url.find("{id}");
        if (pos != std::string::npos) url.replace(pos, 4, std::to_string(id));

        g_listener.bind([id](web::WebTask::Event* e) {
            if (auto res = e->getValue()) {
                std::string body = res->string().unwrapOr("");
                if (body.size() > 300) body = body.substr(0, 300) + "...";
                FLAlertLayer::create(
                    "Level History",
                    fmt::format("ID {}\nCodigo: {}\n\n{}", id, res->code(), clean(body)),
                    "OK"
                )->show();
            }
        });
        g_listener.setFilter(web::WebRequest().get(url));
        Notification::create("Buscando nivel...", NotificationIcon::Loading)->show();
    }
}

class $modify(HistoryBrowser, LevelBrowserLayer) {
    bool init(GJSearchObject* obj) {
        if (!LevelBrowserLayer::init(obj)) return false;

        auto spr = ButtonSprite::create("History");
        auto btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(HistoryBrowser::onHistory));

        auto winSize = CCDirector::sharedDirector()->getWinSize();
        auto menu = CCMenu::create();
        menu->addChild(btn);
        menu->setPosition({ 70.f, winSize.height - 100.f });
        this->addChild(menu, 100);
        return true;
    }

    void onHistory(CCObject*) {
        std::string clip = utils::string::trim(clipboard::read());
        int id = utils::numFromString<int>(clip).unwrapOr(0);
        if (id <= 0) {
            FLAlertLayer::create(
                "Level History",
                "Copia primero el ID de un nivel y vuelve a tocar el boton.",
                "OK"
            )->show();
            return;
        }
        fetchLevel(id);
    }
};
