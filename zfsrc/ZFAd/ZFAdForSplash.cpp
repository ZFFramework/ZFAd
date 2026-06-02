#include "ZFAdForSplash.h"
#include "protocol/ZFProtocolZFAdForSplash.h"

ZF_NAMESPACE_GLOBAL_BEGIN

zfclassNotPOD _ZFP_ZFAdForSplashPrivate {
public:
    zfautoT<ZFAdForSplashImpl> impl;
    void *nativeAd;
    zfweakT<ZFUIRootWindow> window;
    zfauto eventHolder;
    ZFCoreArray<ZFListener> onLoadStopList;
    zfbool loading;
    zfbool started;
public:
    _ZFP_ZFAdForSplashPrivate(void)
    : impl()
    , nativeAd(zfnull)
    , window()
    , eventHolder()
    , onLoadStopList()
    , loading(zffalse)
    , started(zffalse)
    {
    }
public:
    void stop(void) {
        this->started = zffalse;
        if(this->eventHolder) {
            ZFObserverGroupRemove(this->eventHolder);
            this->eventHolder = zfnull;
        }
    }
};

// ============================================================
ZFOBJECT_REGISTER(ZFAdForSplash)

ZFEVENT_REGISTER(ZFAdForSplash, AdOnDisplay)
ZFEVENT_REGISTER(ZFAdForSplash, AdOnClick)

ZFEVENT_REGISTER(ZFAdForSplash, AdOnLoadStop)
ZFEVENT_REGISTER(ZFAdForSplash, AdOnStart)
ZFEVENT_REGISTER(ZFAdForSplash, AdOnStop)

ZFMETHOD_DEFINE_3(ZFAdForSplash, void, setup
        , ZFMP_IN(const zfstring &, implName)
        , ZFMP_IN(const zfstring &, appId)
        , ZFMP_IN(const zfstring &, adId)
        ) {
    if(d->nativeAd) {
        d->impl->nativeAdDestroy(this);
        d->nativeAd = zfnull;
        d->impl = zfnull;
    }

    const ZFClass *cls = ZFClass::classForName(zfstr("ZFAdForSplashImpl_%s", implName));
    if(cls == zfnull) {
        ZFLogTrim("[ZFAdForSplash] no impl: %s", implName);
        return;
    }
    zfautoT<ZFAdForSplashImpl> impl = cls->newInstance();
    if(zfcast(ZFAdForSplashImpl *, impl) == zfnull) {
        ZFLogTrim("[ZFAdForSplash] invalid impl: %s", cls);
        return;
    }

    void *nativeAd = impl->nativeAdCreate(this, appId, adId);
    if(nativeAd == zfnull) {
        ZFLogTrim("[ZFAdForSplash] unable to setup(%s, %s, %s)", implName, appId, adId);
        return;
    }

    d->impl = impl;
    d->nativeAd = nativeAd;
}

ZFMETHOD_DEFINE_0(ZFAdForSplash, void *, nativeAd) {
    return d->nativeAd;
}

ZFMETHOD_DEFINE_0(ZFAdForSplash, ZFUIRootWindow *, window) {
    ZFUIRootWindow *ret = d->window;
    if(ret != zfnull) {
        return ret;
    }
    else {
        return ZFUIRootWindow::mainWindow();
    }
}
ZFMETHOD_DEFINE_1(ZFAdForSplash, void, window
        , ZFMP_IN(ZFUIRootWindow *, window)
        ) {
    d->window = window;
}

ZFMETHOD_DEFINE_1(ZFAdForSplash, zfautoT<ZFTaskId>, load
        , ZFMP_IN_OPT(const ZFListener &, onLoadStop, zfnull)
        ) {
    if(d->nativeAd == zfnull) {
        zfobj<v_zfstring> errorHint("ad has not been setup");
        this->observerNotify(zfself::E_AdOnLoadStop(), zfobj<v_ZFResultType>(v_ZFResultType::e_Fail), errorHint);
        if(onLoadStop) {
            onLoadStop.execute(ZFArgs()
                    .sender(this)
                    .param0(zfobj<v_ZFResultType>(v_ZFResultType::e_Fail))
                    .param1(errorHint)
                    );
        }
        return zfnull;
    }

    if(this->loaded()) {
        onLoadStop.execute(ZFArgs()
                .sender(this)
                .param0(zfobj<v_ZFResultType>(v_ZFResultType::e_Success))
                .param1(zfobj<v_zfstring>())
                );
        return zfnull;
    }
    if(!onLoadStop) {
        if(!d->loading) {
            d->loading = zftrue;
            zfobjRetain(this);
            d->impl->nativeAdLoad(this);
        }
        return zfnull;
    }

    zfweakT<zfself> owner = this;
    zfobj<ZFTaskIdBasic> taskId;

    ZFLISTENER_2(onLoadedWrap
            , zfweakT<ZFTaskIdBasic>, taskId
            , ZFListener, onLoadStop
            ) {
        if(taskId) {
            taskId->stopImpl(zfnull);
        }
        onLoadStop.execute(zfargs);
    } ZFLISTENER_END()

    ZFLISTENER_2(stopImpl
            , zfweakT<zfself>, owner
            , ZFListener, onLoadedWrap
            ) {
        if(owner) {
            owner->d->onLoadStopList.removeElement(onLoadedWrap);
        }
    } ZFLISTENER_END()
    taskId->stopImpl(stopImpl);

    d->onLoadStopList.add(onLoadedWrap);

    if(!d->loading) {
        d->loading = zftrue;
        zfobjRetain(this);
        d->impl->nativeAdLoad(this);
    }

    return taskId;
}
ZFMETHOD_DEFINE_0(ZFAdForSplash, zfbool, loaded) {
    return d->impl && d->impl->nativeAdLoaded(this);
}

ZFMETHOD_DEFINE_0(ZFAdForSplash, void, start) {
    if(d->started) {
        return;
    }
    d->started = zftrue;
    zfobjRetain(this);
    this->observerNotify(zfself::E_AdOnStart());
    if(d->nativeAd == zfnull) {
        zfobj<v_zfstring> errorHint("ad has not been setup");
        this->observerNotify(zfself::E_AdOnStop(), errorHint);
        return;
    }

    zfweakT<zfself> owner = this;
    ZFLISTENER_1(onLoad
            , zfweakT<zfself>, owner
            ) {
        if(!owner->loaded()) {
            owner->_ZFP_ZFAdForSplash_stop(v_ZFResultType::e_Fail, "load failed");
            return;
        }

        ZFUIRootWindow *window = owner->window();
        if(window->windowCreated()) {
            owner->d->impl->nativeAdStart(owner);
        }
        else {
            owner->d->eventHolder = zfobj<ZFObject>();
            ZFLISTENER_1(windowOnCreate
                    , zfweakT<zfself>, owner
                    ) {
                if(owner->d->eventHolder) {
                    ZFObserverGroupRemove(owner->d->eventHolder);
                    owner->d->eventHolder = zfnull;
                }
                owner->d->impl->nativeAdStart(owner);
            } ZFLISTENER_END()
            ZFObserverGroup(owner->d->eventHolder, window)
                .observerAdd(ZFUIRootWindow::E_WindowOnCreate(), windowOnCreate, ZFLevelZFFrameworkPostNormal)
                ;
        }
    } ZFLISTENER_END()
    this->load(onLoad);
}
ZFMETHOD_DEFINE_0(ZFAdForSplash, zfbool, started) {
    return d->started;
}

ZFOBJECT_ON_INIT_DEFINE_3(ZFAdForSplash
        , ZFMP_IN(const zfstring &, implName)
        , ZFMP_IN(const zfstring &, appId)
        , ZFMP_IN(const zfstring &, adId)
        ) {
    this->setup(implName, appId, adId);
}

void ZFAdForSplash::objectOnInit(void) {
    zfsuper::objectOnInit();
    d = zfpoolNew(_ZFP_ZFAdForSplashPrivate);
}
void ZFAdForSplash::objectOnDealloc(void) {
    zfpoolDelete(d);
    zfsuper::objectOnDealloc();
}
void ZFAdForSplash::objectOnDeallocPrepare(void) {
    if(d->nativeAd) {
        d->impl->nativeAdDestroy(this);
        d->nativeAd = zfnull;
        d->impl = zfnull;
    }
    zfsuper::objectOnDeallocPrepare();
}

void *ZFAdForSplash::_ZFP_ZFAdForSplash_impl(void) {
    return (void *)d->impl.to<ZFAdForSplashImpl *>();
}
void ZFAdForSplash::_ZFP_ZFAdForSplash_onLoadStop(
        ZF_IN ZFResultType resultType
        , ZF_IN const zfstring &errorHint
        ) {
    if(d->loading) {
        d->loading = zffalse;
        zfobj<v_ZFResultType> resultTypeTmp(resultType);
        zfobj<v_zfstring> errorHintTmp;
        if(!d->onLoadStopList.isEmpty()) {
            ZFCoreArray<ZFListener> tmp;
            tmp.swap(d->onLoadStopList);
            ZFArgs zfargs;
            zfargs
                .sender(this)
                .param0(resultTypeTmp)
                .param1(errorHintTmp)
                ;
            for(zfindex i = 0; i < tmp.count(); ++i) {
                tmp[i].execute(zfargs);
            }
        }
        this->observerNotify(zfself::E_AdOnLoadStop(), resultTypeTmp, errorHintTmp);
        zfobjRelease(this);
    }
}
void ZFAdForSplash::_ZFP_ZFAdForSplash_stop(
        ZF_IN ZFResultType resultType
        , ZF_IN const zfstring &errorHint
        ) {
    if(d->started) {
        d->stop();
        this->observerNotify(zfself::E_AdOnStop(), zfobj<v_ZFResultType>(resultType), zfobj<v_zfstring>(errorHint));
        zfobjRelease(this);
    }
}

ZF_NAMESPACE_GLOBAL_END

