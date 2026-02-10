#include "ZFAdForBannerHelper.h"

ZF_NAMESPACE_GLOBAL_BEGIN

zfclassNotPOD _ZFP_ZFAdForBannerHelperPrivate {
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
    zfautoT<ZFAdForBanner> impl; // current impl, may be null
    zfindex nextIndex; // next index to try, may exceed cfgList
    zfobj<ZFObject> observerOwner;
    zftimet closeTime;
    zfautoT<ZFTaskId> closeTimeoutTaskId;
public:
    _ZFP_ZFAdForBannerHelperPrivate(void)
    : cfgList()
    , impl()
    , nextIndex(0)
    , observerOwner()
    , closeTime(zftimetInvalid())
    , closeTimeoutTaskId()
    {
    }
public:
    void implStop(void) {
        if(this->impl) {
            ZFObserverGroupRemove(this->observerOwner);
            this->impl->removeFromParent();
            this->impl = zfnull;
        }
    }
    void implUpdate(ZF_IN ZFAdForBannerHelper *owner) {
        if(this->impl || this->nextIndex >= this->cfgList.count()) {
            return;
        }
        if(this->closeTime != zftimetInvalid()) {
            if(this->closeTimeoutTaskId == zfnull) {
                ZFLISTENER_1(closeTimeout
                        , zfweakT<ZFAdForBannerHelper>, owner
                        ) {
                    owner->d->closeTime = zftimetInvalid();
                    owner->d->closeTimeoutTaskId = zfnull;
                    owner->d->implUpdate(owner);
                } ZFLISTENER_END()
                zftimet timeout = this->closeTime + owner->closeDuration() - ZFTime::currentTime();
                if(timeout < 0) {
                    timeout = 0;
                }
                this->closeTimeoutTaskId = ZFTimerOnce(owner->closeDuration(), closeTimeout);
            }
            return;
        }
        _ZFP_ZFAdForBannerHelperPrivate::Cfg const &cfg = this->cfgList[(this->nextIndex)++];
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
            this->implUpdate(owner);
            return;
        }

        zfobj<ZFAdForBanner> impl;
        this->impl = impl;
        owner->child(impl)->sizeFill();

        zfweakT<ZFAdForBannerHelper> weakOwner = owner;

        ZFLISTENER_1(AdOnError
                , zfweakT<ZFAdForBannerHelper>, weakOwner
                ) {
            if(!weakOwner) {return;}
            weakOwner->d->implStop();
            weakOwner->d->implUpdate(weakOwner);
        } ZFLISTENER_END()

        ZFLISTENER_1(AdOnDisplay
                , zfweakT<ZFAdForBannerHelper>, weakOwner
                ) {
            if(!weakOwner) {return;}
            weakOwner->observerNotify(ZFAdForBanner::E_AdOnDisplay(), zfargs.param0(), zfargs.param1());
        } ZFLISTENER_END()

        ZFLISTENER_1(AdOnClick
                , zfweakT<ZFAdForBannerHelper>, weakOwner
                ) {
            if(!weakOwner) {return;}
            weakOwner->observerNotify(ZFAdForBanner::E_AdOnClick(), zfargs.param0(), zfargs.param1());
        } ZFLISTENER_END()

        ZFLISTENER_1(AdOnClose
                , zfweakT<ZFAdForBannerHelper>, weakOwner
                ) {
            if(!weakOwner) {return;}
            weakOwner->d->nextIndex = 0;
            weakOwner->d->closeTime = ZFTime::currentTime();
            weakOwner->d->implStop();
            weakOwner->layoutRequest();
            if(weakOwner->viewTreeInWindow()) {
                weakOwner->d->implUpdate(weakOwner);
            }
            weakOwner->observerNotify(ZFAdForBanner::E_AdOnClose(), zfargs.param0(), zfargs.param1());
        } ZFLISTENER_END()

        ZFObserverGroup(this->observerOwner, impl)
            .observerAdd(ZFAdForBanner::E_AdOnError(), AdOnError)
            .observerAdd(ZFAdForBanner::E_AdOnDisplay(), AdOnDisplay)
            .observerAdd(ZFAdForBanner::E_AdOnClick(), AdOnClick)
            .observerAdd(ZFAdForBanner::E_AdOnClose(), AdOnClose)
            ;

        impl->setup(cfg.implName, cfg.appId, cfg.adId);
    }
};

// ============================================================
ZFOBJECT_REGISTER(ZFAdForBannerHelper)

ZFMETHOD_DEFINE_6(ZFAdForBannerHelper, ZFAdForBannerHelper *, cfg
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
    _ZFP_ZFAdForBannerHelperPrivate::Cfg cfg;
    cfg.implName = implName;
    cfg.appId = appId;
    cfg.adId = adId;
    cfg.systemName = systemName;
    cfg.localeId = localeId;
    cfg.localeLangId = localeLangId;
    d->cfgList.add(cfg);

    if(this->viewTreeInWindow()) {
        d->implUpdate(this);
    }
    return this;
}

void ZFAdForBannerHelper::objectOnInit(void) {
    zfsuper::objectOnInit();
    d = zfpoolNew(_ZFP_ZFAdForBannerHelperPrivate);
}
void ZFAdForBannerHelper::objectOnDealloc(void) {
    zfpoolDelete(d);
    zfsuper::objectOnDealloc();
}
void ZFAdForBannerHelper::objectOnDeallocPrepare(void) {
    d->nextIndex = 0;
    if(d->closeTimeoutTaskId) {
        d->closeTimeoutTaskId->stop();
        d->closeTimeoutTaskId = zfnull;
    }
    d->closeTime = zftimetInvalid();
    d->implStop();
    zfsuper::objectOnDeallocPrepare();
}

void ZFAdForBannerHelper::layoutOnMeasure(
        ZF_OUT ZFUISize &ret
        , ZF_IN const ZFUISize &sizeHint
        , ZF_IN const ZFUISizeParam &sizeParam
        ) {
    if(d->impl) {
        ret = d->impl->layoutMeasure(sizeHint, sizeParam);
    }
    else {
        ret = ZFUISizeZero();
    }
}

void ZFAdForBannerHelper::viewTreeInWindowOnUpdate(void) {
    zfsuper::viewTreeInWindowOnUpdate();
    if(this->viewTreeInWindow()) {
        if(d->impl == zfnull) {
            d->implUpdate(this);
        }
    }
}

ZF_NAMESPACE_GLOBAL_END

