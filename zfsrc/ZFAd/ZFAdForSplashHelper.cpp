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
        zfstring localeName;
        zfstring localeLangName;
    };
public:
    ZFCoreArray<Cfg> cfgList;
    zfautoT<ZFAdForSplash> impl; // current impl, may be null
    zfindex index; // next index to try, may exceed cfgList
    zfobj<ZFObject> observerOwner;

    zfweakT<ZFUIRootWindow> window;
    zftimet startTime; // 0 when not started
public:
    _ZFP_ZFAdForSplashHelperPrivate(void)
    : cfgList()
    , impl()
    , index(0)
    , observerOwner()
    , window()
    , startTime()
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

ZFMETHOD_DEFINE_6(ZFAdForSplashHelper, ZFAdForSplashHelper *, cfg
        , ZFMP_IN(const zfstring &, implName)
        , ZFMP_IN(const zfstring &, appId)
        , ZFMP_IN(const zfstring &, adId)
        , ZFMP_IN_OPT(const zfstring &, systemName, zfnull)
        , ZFMP_IN_OPT(const zfstring &, localeName, zfnull)
        , ZFMP_IN_OPT(const zfstring &, localeLangName, zfnull)
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
    cfg.localeName = localeName;
    cfg.localeLangName = localeLangName;
    d->cfgList.add(cfg);
    return this;
}

ZFMETHOD_DEFINE_0(ZFAdForSplashHelper, zfanyT<ZFUIRootWindow>, window) {
    return d->window;
}
ZFMETHOD_DEFINE_1(ZFAdForSplashHelper, ZFAdForSplashHelper *, window
        , ZFMP_IN(ZFUIRootWindow *, v)
        ) {
    d->window = v;
    return this;
}

ZFMETHOD_DEFINE_2(ZFAdForSplashHelper, void, start
        , ZFMP_IN_OPT(const ZFListener &, onStop, zfnull)
        , ZFMP_IN_OPT(ZFUIRootWindow *, window, zfnull)
        ) {
    if(this->started()) {
        return;
    }
    d->startTime = ZFTime::currentTime();
    this->observerNotify(ZFAdForSplash::E_AdOnStart(), d->window ? d->window.toObject() : ZFUIRootWindow::mainWindow().toObject());

    zfclassNotPOD _Impl {
    public:
        static void tryNext(ZF_IN ZFAdForSplashHelper *owner) {
            if(owner->d->impl || owner->d->index >= owner->d->cfgList.count()) {
                return;
            }
            if(ZFTime::currentTime() - owner->d->startTime >= owner->timeout()) {
                owner->d->index = 0;
                owner->d->startTime = 0;
                owner->d->implStop();
                owner->observerNotify(ZFAdForSplash::E_AdOnStop(), zfobj<v_ZFResultType>(v_ZFResultType::e_Fail), zfobj<v_zfstring>("load timeout"));
                return;
            }
            _ZFP_ZFAdForSplashHelperPrivate::Cfg const &cfg = owner->d->cfgList[(owner->d->index)++];
            zfbool valid = zffalse;
            do {
                if(cfg.systemName) {
                    zfstring systemName = ZFEnvInfo::systemName();
                    if(systemName && systemName != cfg.systemName) {
                        break;
                    }
                }

                if(cfg.localeName) {
                    zfstring localeName = ZFEnvInfo::localeInfo();
                    if(localeName && localeName != cfg.localeName) {
                        break;
                    }
                }

                if(cfg.localeLangName) {
                    zfstring localeLangName = ZFEnvInfo::localeLangInfo();
                    if(localeLangName && localeLangName != cfg.localeLangName) {
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

            zfweakT<ZFAdForSplashHelper> weakOwner;

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

            ZFObserverGroup(owner->d->observerOwner, impl)
                .observerAdd(ZFAdForSplash::E_AdOnError(), AdOnError)
                .observerAdd(ZFAdForSplash::E_AdOnDisplay(), AdOnDisplay)
                .observerAdd(ZFAdForSplash::E_AdOnClick(), AdOnClick)
                ;

            impl->setup(cfg.implName, cfg.appId, cfg.adId);
            impl->start();
        }
    };
    _Impl::tryNext(this);
}
ZFMETHOD_DEFINE_0(ZFAdForSplashHelper, zfbool, started) {
    return d->startTime != 0;
}

ZFOBJECT_ON_INIT_DEFINE_1(ZFAdForSplashHelper
        , ZFMP_IN(ZFUIRootWindow *, window)
        ) {
    this->window(window);
}

void ZFAdForSplashHelper::objectOnInit(void) {
    zfsuper::objectOnInit();
    d = zfpoolNew(_ZFP_ZFAdForSplashHelperPrivate);
}
void ZFAdForSplashHelper::objectOnDealloc(void) {
    zfpoolDelete(d);
    zfsuper::objectOnDealloc();
}
void ZFAdForSplashHelper::objectOnDeallocPrepare(void) {
    zfbool needNotify = this->started();
    d->index = 0;
    d->startTime = 0;
    d->implStop();
    if(needNotify) {
    }
    zfsuper::objectOnDeallocPrepare();
}

ZF_NAMESPACE_GLOBAL_END

