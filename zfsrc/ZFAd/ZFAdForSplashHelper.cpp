#include "ZFAdForSplashHelper.h"

ZF_NAMESPACE_GLOBAL_BEGIN

zfclassNotPOD _ZFP_ZFAdForSplashHelperPrivate {
public:
    zfclassLikePOD Cfg {
    public:
        zfstring implName;
        zfstring appId;
        zfstring adId;
        zfstring systemName;
        zfstring localeId;
        zfstring localeLangId;
    };
public:
    ZFCoreArray<Cfg> cfgList;
    zfautoT<ZFAdForSplash> impl; // current impl, may be null
    zfindex index; // next index to try, may exceed cfgList

    zfobj<ZFObject> observerOwner;
    zfweakT<ZFUIRootWindow> window;
    zftimet startTime; // 0 when not started
    zfautoT<ZFUIWindow> bgWindow;
    zfauto holder; // retained during running

    zftimet silentDurationBegin; // last time has showed ad
    zfauto attachObserverOwner; // not null if attached
    zfauto attachHolder; // retained during attached

public:
    _ZFP_ZFAdForSplashHelperPrivate(void)
    : cfgList()
    , impl()
    , index(0)
    , observerOwner()
    , window()
    , startTime()
    , bgWindow()
    , holder()
    , silentDurationBegin(zftimetInvalid())
    , attachObserverOwner()
    , attachHolder()
    {
    }
public:
    void implStop(void) {
        if(this->impl) {
            ZFObserverGroupRemove(this->observerOwner);
            this->impl = zfnull;
        }
    }
};

// ============================================================
ZFOBJECT_REGISTER(ZFAdForSplashHelper)

ZFMETHOD_DEFINE_1(ZFAdForSplashHelper, zfautoT<ZFAdForSplashHelper>, instance
        , ZFMP_IN_OPT(ZFUIRootWindow *, window, zfnull)
        ) {
    if(window == zfnull) {
        window = ZFUIRootWindow::mainWindow();
    }
    zfautoT<ZFAdForSplashHelper> ret = window->objectTag("_ZFP_ZFAdForSplashHelper_instance");
    if(!ret) {
        ret = zfobj<ZFAdForSplashHelper>();
        ret->d->window = window;
        window->objectTag("_ZFP_ZFAdForSplashHelper_instance", ret);
    }
    return ret;
}

ZFMETHOD_DEFINE_6(ZFAdForSplashHelper, ZFAdForSplashHelper *, cfg
        , ZFMP_IN(const zfstring &, implName)
        , ZFMP_IN(const zfstring &, appId)
        , ZFMP_IN(const zfstring &, adId)
        , ZFMP_IN_OPT(const zfstring &, systemName, zfnull)
        , ZFMP_IN_OPT(const zfstring &, localeId, zfnull)
        , ZFMP_IN_OPT(const zfstring &, localeLangId, zfnull)
        ) {
    if(zffalse
            || !implName
            || !appId
            || !adId
            ) {
        return this;
    }
    _ZFP_ZFAdForSplashHelperPrivate::Cfg cfg;
    cfg.implName = implName;
    cfg.appId = appId;
    cfg.adId = adId;
    cfg.systemName = systemName;
    cfg.localeId = localeId;
    cfg.localeLangId = localeLangId;
    d->cfgList.add(cfg);
    return this;
}

ZFMETHOD_DEFINE_0(ZFAdForSplashHelper, zfanyT<ZFUIRootWindow>, window) {
    return d->window ? d->window.get() : ZFUIRootWindow::mainWindow();
}

ZFMETHOD_DEFINE_1(ZFAdForSplashHelper, void, start
        , ZFMP_IN_OPT(const ZFListener &, onStop, zfnull)
        ) {
    if(this->started()) {
        return;
    }
    d->holder = this;
    d->bgWindow = zfobj<ZFUIWindow>(this->window());
    d->bgWindow->viewId("ZFAdForSplashBg");
    d->bgWindow->bgColor(ZFUIColorCreateRGB(0xFFFFFF));
    d->bgWindow->windowLevel(ZFUIWindowLevelOverlay() - 1);
    d->bgWindow->windowMarginShouldApply(zffalse);
    if(this->bg()) {
        this->bg()->removeFromParent();
        d->bgWindow->child(this->bg())->sizeFill();
    }
    d->bgWindow->show();

    d->startTime = ZFTime::currentTime();
    this->observerNotify(ZFAdForSplash::E_AdOnStart(), this->window());

    zfclassNotPOD _Impl {
    public:
        static void tryNext(ZF_IN ZFAdForSplashHelper *owner) {
            if(owner->d->impl || owner->d->index >= owner->d->cfgList.count()) {
                _stop(owner, zfobj<v_ZFResultType>(v_ZFResultType::e_Fail), zfobj<v_zfstring>("no valid impl"));
                return;
            }
            if(ZFTime::currentTime() - owner->d->startTime >= owner->timeout()) {
                _stop(owner, zfobj<v_ZFResultType>(v_ZFResultType::e_Fail), zfobj<v_zfstring>("load timeout"));
                return;
            }

            _ZFP_ZFAdForSplashHelperPrivate::Cfg const &cfg = owner->d->cfgList[(owner->d->index)++];
            zfbool valid = zffalse;
            do {
                if(cfg.systemName) {
                    zfstring systemName = ZFEnvInfo::systemName();
                    if(systemName && !ZFRegExpMatch(systemName, cfg.systemName)) {
                        break;
                    }
                }

                if(cfg.localeId) {
                    zfstring localeId = ZFEnvInfo::localeId();
                    if(localeId && !ZFRegExpMatch(localeId, cfg.localeId)) {
                        break;
                    }
                }

                if(cfg.localeLangId) {
                    zfstring localeLangId = ZFEnvInfo::localeLangId();
                    if(localeLangId && !ZFRegExpMatch(localeLangId, cfg.localeLangId)) {
                        break;
                    }
                }

                valid = zftrue;
            } while(zffalse);
            if(!valid) {
                tryNext(owner);
                return;
            }

            zfobj<ZFAdForSplash> impl;
            owner->d->impl = impl;

            zfweakT<ZFAdForSplashHelper> weakOwner = owner;

            ZFLISTENER_1(AdOnError
                    , zfweakT<ZFAdForSplashHelper>, weakOwner
                    ) {
                if(!weakOwner) {return;}
                weakOwner->d->implStop();
                tryNext(weakOwner);
            } ZFLISTENER_END()

            ZFLISTENER_1(AdOnDisplay
                    , zfweakT<ZFAdForSplashHelper>, weakOwner
                    ) {
                if(!weakOwner) {return;}
                weakOwner->observerNotify(ZFAdForSplash::E_AdOnDisplay(), zfargs.param0(), zfargs.param1());
            } ZFLISTENER_END()

            ZFLISTENER_1(AdOnClick
                    , zfweakT<ZFAdForSplashHelper>, weakOwner
                    ) {
                if(!weakOwner) {return;}
                weakOwner->observerNotify(ZFAdForSplash::E_AdOnClick(), zfargs.param0(), zfargs.param1());
            } ZFLISTENER_END()

            ZFLISTENER_1(AdOnStop
                    , zfweakT<ZFAdForSplashHelper>, weakOwner
                    ) {
                if(!weakOwner) {return;}
                _stop(weakOwner, zfargs.param0(), zfargs.param1());
            } ZFLISTENER_END()

            ZFObserverGroup(owner->d->observerOwner, impl)
                .observerAdd(ZFAdForSplash::E_AdOnError(), AdOnError)
                .observerAdd(ZFAdForSplash::E_AdOnDisplay(), AdOnDisplay)
                .observerAdd(ZFAdForSplash::E_AdOnClick(), AdOnClick)
                .observerAdd(ZFAdForSplash::E_AdOnStop(), AdOnStop)
                ;

            impl->setup(cfg.implName, cfg.appId, cfg.adId);
            impl->start();
        }
    private:
        static void _stop(
                ZF_IN ZFAdForSplashHelper *owner
                , ZF_IN v_ZFResultType *resultType
                , ZF_IN v_zfstring *errorHint
                ) {
            owner->d->index = 0;
            owner->d->startTime = 0;
            owner->d->implStop();
            if(owner->d->bgWindow) {
                owner->d->bgWindow->hide();
                owner->d->bgWindow = zfnull;
            }
            if(resultType->zfv() == v_ZFResultType::e_Fail) {
                owner->observerNotify(ZFAdForSplash::E_AdOnError(), errorHint);
            }
            owner->observerNotify(ZFAdForSplash::E_AdOnStop(), resultType, errorHint);
            owner->d->holder = zfnull;
        }
    };
    _Impl::tryNext(this);
}
ZFMETHOD_DEFINE_0(ZFAdForSplashHelper, zfbool, started) {
    return d->startTime != 0;
}

void ZFAdForSplashHelper::objectOnInit(void) {
    zfsuper::objectOnInit();
    d = zfpoolNew(_ZFP_ZFAdForSplashHelperPrivate);
}
void ZFAdForSplashHelper::objectOnDealloc(void) {
    zfpoolDelete(d);
    zfsuper::objectOnDealloc();
}

// ============================================================
ZFMETHOD_DEFINE_0(ZFAdForSplashHelper, void, attach) {
    zfobjReleaseInScope(zfobjRetain(this));
    this->detach();

    d->attachObserverOwner = zfobj<ZFObject>();
    d->attachHolder = this;
    zfself *owner = this;
    ZFLISTENER_1(windowOnShow
            , zfweakT<zfself>, owner
            ) {
        zftimet curTime = ZFTime::currentTime();
        if(owner->d->silentDurationBegin == zftimetInvalid()
                || curTime - owner->d->silentDurationBegin > owner->silentDuration()
                ) {
            owner->d->silentDurationBegin = curTime;
            owner->start();
        }
    } ZFLISTENER_END()
    ZFObserverGroup(d->attachObserverOwner, this->window())
        .observerAdd(ZFUIRootWindow::E_WindowOnResume(), windowOnShow)
        ;
    if(this->window()->windowResumed()) {
        windowOnShow.execute();
    }
}
ZFMETHOD_DEFINE_0(ZFAdForSplashHelper, void, detach) {
    if(d->attachObserverOwner) {
        ZFObserverGroupRemove(d->attachObserverOwner);
        d->attachObserverOwner = zfnull;
        d->attachHolder = zfnull;
    }
}
ZFMETHOD_DEFINE_0(ZFAdForSplashHelper, zfbool, attached) {
    return d->attachObserverOwner;
}

ZF_NAMESPACE_GLOBAL_END

