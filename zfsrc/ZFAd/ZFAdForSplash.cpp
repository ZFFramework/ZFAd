#include "ZFAdForSplash.h"
#include "protocol/ZFProtocolZFAdForSplash.h"

ZF_NAMESPACE_GLOBAL_BEGIN

zfclassNotPOD _ZFP_ZFAdForSplashPrivate {
public:
    void *nativeAd;
    zfbool started;
public:
    _ZFP_ZFAdForSplashPrivate(void)
    : nativeAd(zfnull)
    , started(zffalse)
    {
    }
};

// ============================================================
ZFOBJECT_REGISTER(ZFAdForSplash)

ZFEVENT_REGISTER(ZFAdForSplash, AdOnError)
ZFEVENT_REGISTER(ZFAdForSplash, AdOnDisplay)
ZFEVENT_REGISTER(ZFAdForSplash, AdOnClick)

ZFEVENT_REGISTER(ZFAdForSplash, AdOnStart)
ZFEVENT_REGISTER(ZFAdForSplash, AdOnTimer)
ZFEVENT_REGISTER(ZFAdForSplash, AdOnStop)

ZFPROPERTY_ON_UPDATE_DEFINE(ZFAdForSplash, zfstring, appId) {
    ZFPROTOCOL_ACCESS(ZFAdForSplash)->appIdUpdate(this);
}
ZFPROPERTY_ON_UPDATE_DEFINE(ZFAdForSplash, zfstring, adId) {
    ZFPROTOCOL_ACCESS(ZFAdForSplash)->adIdUpdate(this);
}

ZFMETHOD_DEFINE_0(ZFAdForSplash, void *, nativeAd) {
    return d->nativeAd;
}

ZFMETHOD_DEFINE_1(ZFAdForSplash, void, start
        , ZFMP_IN_OPT(const ZFListener &, onStop, zfnull)
        ) {
    if(!d->started) {
        d->started = zftrue;
        zfRetain(this);
        ZFPROTOCOL_ACCESS(ZFAdForSplash)->start(this);
        this->observerNotify(zfself::E_AdOnStart());
    }
}
ZFMETHOD_DEFINE_0(ZFAdForSplash, void, stop) {
    if(d->started) {
        d->started = zffalse;
        ZFPROTOCOL_ACCESS(ZFAdForSplash)->stop(this);
        this->observerNotify(zfself::E_AdOnStop(), zfobj<v_ZFResultType>(v_ZFResultType::e_Cancel));
        zfRelease(this);
    }
}
ZFMETHOD_DEFINE_0(ZFAdForSplash, zfbool, started) {
    return d->started;
}

void ZFAdForSplash::objectOnInit(void) {
    zfsuper::objectOnInit();
    d = zfpoolNew(_ZFP_ZFAdForSplashPrivate);
    d->nativeAd = ZFPROTOCOL_ACCESS(ZFAdForSplash)->nativeAdCreate(this);
}
void ZFAdForSplash::objectOnDealloc(void) {
    ZFPROTOCOL_ACCESS(ZFAdForSplash)->nativeAdDestroy(this);
    zfpoolDelete(d);
    zfsuper::objectOnDealloc();
}

void ZFAdForSplash::_ZFP_ZFAdForSplash_stop(
        ZF_IN ZFResultType resultType
        , ZF_IN const zfstring &errorHint
        ) {
    if(d->started) {
        d->started = zffalse;
        if(resultType == v_ZFResultType::e_Fail) {
            this->observerNotify(zfself::E_AdOnError(), zfobj<v_zfstring>(errorHint));
        }
        this->observerNotify(zfself::E_AdOnStop(), zfobj<v_ZFResultType>(resultType), zfobj<v_zfstring>(errorHint));
        zfRelease(this);
    }
}

ZF_NAMESPACE_GLOBAL_END

