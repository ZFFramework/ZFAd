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
        zfstring localeName;
        zfstring localeLangName;
    };
public:
    ZFCoreArray<Cfg> cfgList;
    zfautoT<ZFAdForBanner> impl; // current impl, may be null
    zfindex index; // next index to try, may exceed cfgList
    zfobj<ZFObject> observerOwner;
public:
    _ZFP_ZFAdForBannerHelperPrivate(void)
    : cfgList()
    , impl()
    , index(0)
    , observerOwner()
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
};

// ============================================================
ZFOBJECT_REGISTER(ZFAdForBannerHelper)

ZFMETHOD_DEFINE_6(ZFAdForBannerHelper, ZFAdForBannerHelper *, cfg
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
    _ZFP_ZFAdForBannerHelperPrivate::Cfg cfg;
    cfg.implName = implName;
    cfg.appId = appId;
    cfg.adId = adId;
    cfg.systemName = systemName;
    cfg.localeName = localeName;
    cfg.localeLangName = localeLangName;
    d->cfgList.add(cfg);

    zfclassNotPOD _Impl {
    public:
        static void tryNext(ZF_IN ZFAdForBannerHelper *owner) {
            if(owner->d->impl || owner->d->index >= owner->d->cfgList.count()) {
                return;
            }
            _ZFP_ZFAdForBannerHelperPrivate::Cfg const &cfg = owner->d->cfgList[(owner->d->index)++];
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

            zfobj<ZFAdForBanner> impl;
            owner->d->impl = impl;
            owner->child(impl)->sizeFill();

            zfweakT<ZFAdForBannerHelper> weakOwner;

            ZFLISTENER_1(AdOnError
                    , zfweakT<ZFAdForBannerHelper>, weakOwner
                    ) {
                if(!weakOwner) {return;}
                weakOwner->d->implStop();
                tryNext(weakOwner);
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
                weakOwner->observerNotify(ZFAdForBanner::E_AdOnClose(), zfargs.param0(), zfargs.param1());
            } ZFLISTENER_END()

            ZFObserverGroup(owner->d->observerOwner, impl)
                .observerAdd(ZFAdForBanner::E_AdOnError(), AdOnError)
                .observerAdd(ZFAdForBanner::E_AdOnDisplay(), AdOnDisplay)
                .observerAdd(ZFAdForBanner::E_AdOnClick(), AdOnClick)
                .observerAdd(ZFAdForBanner::E_AdOnClose(), AdOnClose)
                ;

            impl->setup(cfg.implName, cfg.appId, cfg.adId);
        }
    };
    _Impl::tryNext(this);
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
    d->index = 0;
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

ZF_NAMESPACE_GLOBAL_END

