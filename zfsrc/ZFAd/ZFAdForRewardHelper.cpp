#include "ZFAdForRewardHelper.h"

ZF_NAMESPACE_GLOBAL_BEGIN

zfclassNotPOD _ZFP_ZFAdForRewardHelperPrivate {
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
    ZFAdForRewardHelper *owner;
    ZFCoreArray<Cfg> cfgList;
    zfautoT<ZFAdForReward> impl; // current impl, may be null
    zfindex index; // next index to try, may exceed cfgList

    zfobj<ZFObject> observerOwner;
    zfweakT<ZFUIRootWindow> window;
    zftimet startTime; // 0 when not started
    zfbool implShowing;
    zfauto holder; // retained during running

public:
    _ZFP_ZFAdForRewardHelperPrivate(void)
    : cfgList()
    , impl()
    , index(0)
    , observerOwner()
    , window()
    , startTime()
    , implShowing(zffalse)
    , holder()
    {
    }
public:
    void implStop(void) {
        if(this->impl) {
            ZFObserverGroupRemove(this->observerOwner);
            this->impl = zfnull;
        }
        this->implShowing = zffalse;
    }
};

// ============================================================
ZFOBJECT_REGISTER(ZFAdForRewardHelper)

ZFEVENT_REGISTER(ZFAdForRewardHelper, AdOnCheck)

ZFMETHOD_DEFINE_1(ZFAdForRewardHelper, zfautoT<ZFAdForRewardHelper>, instance
        , ZFMP_IN_OPT(ZFUIRootWindow *, window, zfnull)
        ) {
    if(window == zfnull) {
        window = ZFUIRootWindow::mainWindow();
    }
    zfautoT<ZFAdForRewardHelper> ret = window->objectTag("_ZFP_ZFAdForRewardHelper_instance");
    if(!ret) {
        ret = zfobj<ZFAdForRewardHelper>();
        ret->d->window = window;
        window->objectTag("_ZFP_ZFAdForRewardHelper_instance", ret);
    }
    return ret;
}

ZFMETHOD_DEFINE_6(ZFAdForRewardHelper, ZFAdForRewardHelper *, cfg
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
    _ZFP_ZFAdForRewardHelperPrivate::Cfg cfg;
    cfg.implName = implName;
    cfg.appId = appId;
    cfg.adId = adId;
    cfg.systemName = systemName;
    cfg.localeId = localeId;
    cfg.localeLangId = localeLangId;
    d->cfgList.add(cfg);
    return this;
}

ZFMETHOD_DEFINE_0(ZFAdForRewardHelper, zfanyT<ZFUIRootWindow>, window) {
    return d->window ? d->window.get() : ZFUIRootWindow::mainWindow();
}

ZFMETHOD_DEFINE_1(ZFAdForRewardHelper, void, start
        , ZFMP_IN_OPT(const ZFListener &, onStop, zfnull)
        ) {
    if(this->started()) {
        return;
    }

    d->holder = this;

    d->startTime = ZFTime::currentTime();
    this->observerNotify(ZFAdForReward::E_AdOnStart(), this->window());

    zfclassNotPOD _Impl {
    public:
        static void tryNext(ZF_IN ZFAdForRewardHelper *owner) {
            if(owner->d->impl || owner->d->index >= owner->d->cfgList.count()) {
                _stop(owner, zfobj<v_ZFResultType>(v_ZFResultType::e_Fail), zfobj<v_zfstring>("no valid impl"));
                return;
            }
            if(owner->timeout() != zftimetInvalid()
                    && ZFTime::currentTime() - owner->d->startTime >= owner->timeout()
                    ) {
                _stop(owner, zfobj<v_ZFResultType>(v_ZFResultType::e_Fail), zfobj<v_zfstring>("load timeout"));
                return;
            }

            _ZFP_ZFAdForRewardHelperPrivate::Cfg const &cfg = owner->d->cfgList[(owner->d->index)++];
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

            zfobj<ZFAdForReward> impl;
            owner->d->impl = impl;

            zfweakT<ZFAdForRewardHelper> weakOwner = owner;

            ZFLISTENER_1(AdOnError
                    , zfweakT<ZFAdForRewardHelper>, weakOwner
                    ) {
                if(!weakOwner) {return;}
                weakOwner->d->implShowing = zffalse;
                weakOwner->d->implStop();
                tryNext(weakOwner);
            } ZFLISTENER_END()

            ZFLISTENER_1(AdOnDisplay
                    , zfweakT<ZFAdForRewardHelper>, weakOwner
                    ) {
                if(!weakOwner) {return;}
                weakOwner->d->implShowing = zftrue;
                weakOwner->observerNotify(ZFAdForReward::E_AdOnDisplay(), zfargs.param0(), zfargs.param1());
            } ZFLISTENER_END()

            ZFLISTENER_1(AdOnClick
                    , zfweakT<ZFAdForRewardHelper>, weakOwner
                    ) {
                if(!weakOwner) {return;}
                weakOwner->observerNotify(ZFAdForReward::E_AdOnClick(), zfargs.param0(), zfargs.param1());
            } ZFLISTENER_END()

            ZFLISTENER_1(AdOnStop
                    , zfweakT<ZFAdForRewardHelper>, weakOwner
                    ) {
                if(!weakOwner) {return;}
                weakOwner->d->implShowing = zffalse;
                _stop(weakOwner, zfargs.param0(), zfargs.param1());
            } ZFLISTENER_END()

            ZFObserverGroup(owner->d->observerOwner, impl)
                .observerAdd(ZFAdForReward::E_AdOnError(), AdOnError)
                .observerAdd(ZFAdForReward::E_AdOnDisplay(), AdOnDisplay)
                .observerAdd(ZFAdForReward::E_AdOnClick(), AdOnClick)
                .observerAdd(ZFAdForReward::E_AdOnStop(), AdOnStop)
                ;

            impl->setup(cfg.implName, cfg.appId, cfg.adId);
            impl->start();
        }
    private:
        static void _stop(
                ZF_IN ZFAdForRewardHelper *owner
                , ZF_IN v_ZFResultType *resultType
                , ZF_IN v_zfstring *errorHint
                ) {
            owner->d->index = 0;
            owner->d->startTime = 0;
            owner->d->implStop();
            if(resultType->zfv() == v_ZFResultType::e_Fail) {
                owner->observerNotify(ZFAdForReward::E_AdOnError(), errorHint);
            }
            owner->observerNotify(ZFAdForReward::E_AdOnStop(), resultType, errorHint);
            owner->d->holder = zfnull;
        }
    };
    _Impl::tryNext(this);
}
ZFMETHOD_DEFINE_0(ZFAdForRewardHelper, zfbool, started) {
    return d->startTime != 0;
}

void ZFAdForRewardHelper::objectOnInit(void) {
    zfsuper::objectOnInit();
    d = zfpoolNew(_ZFP_ZFAdForRewardHelperPrivate);
    d->owner = this;
}
void ZFAdForRewardHelper::objectOnDealloc(void) {
    zfpoolDelete(d);
    zfsuper::objectOnDealloc();
}

ZF_NAMESPACE_GLOBAL_END

