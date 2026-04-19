#include "ZFAdForSplash.h"
#include "protocol/ZFProtocolZFAdForSplash.h"

ZF_NAMESPACE_GLOBAL_BEGIN

zfclassNotPOD _ZFP_ZFAdForSplashPrivate {
public:
    zfautoT<ZFAdForSplashImpl> impl;
    void *nativeAd;
    zfweakT<ZFUIRootWindow> window;
    zfauto eventHolder;
    ZFCoreArray<ZFListener> loadCallbacks;
    zfbool loading;
    zfbool started;
public:
    _ZFP_ZFAdForSplashPrivate(void)
    : impl()
    , nativeAd(zfnull)
    , window()
    , eventHolder()
    , loadCallbacks()
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

ZFEVENT_REGISTER(ZFAdForSplash, AdOnError)
ZFEVENT_REGISTER(ZFAdForSplash, AdOnDisplay)
ZFEVENT_REGISTER(ZFAdForSplash, AdOnClick)

ZFEVENT_REGISTER(ZFAdForSplash, AdOnLoad)
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
        , ZFMP_IN_OPT(const ZFListener &, onLoaded, zfnull)
        ) {
    if(d->nativeAd == zfnull) {
        this->observerNotify(zfself::E_AdOnError(), zfobj<v_zfstring>("ad has not been setup"));
        if(onLoaded) {
            onLoaded.execute(ZFArgs().sender(this));
        }
        return zfnull;
    }

    if(this->loaded()) {
        onLoaded.execute(ZFArgs().sender(this));
        return zfnull;
    }
    if(!onLoaded) {
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
            , ZFListener, onLoaded
            ) {
        if(taskId) {
            taskId->stopImpl(zfnull);
        }
        onLoaded.execute(zfargs);
    } ZFLISTENER_END()

    ZFLISTENER_2(stopImpl
            , zfweakT<zfself>, owner
            , ZFListener, onLoadedWrap
            ) {
        if(owner) {
            owner->d->loadCallbacks.removeElement(onLoadedWrap);
        }
    } ZFLISTENER_END()
    taskId->stopImpl(stopImpl);

    d->loadCallbacks.add(onLoadedWrap);

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
    if(d->nativeAd == zfnull) {
        this->observerNotify(zfself::E_AdOnError(), zfobj<v_zfstring>("ad has not been setup"));
        return;
    }
    d->started = zftrue;
    zfobjRetain(this);
    this->observerNotify(zfself::E_AdOnStart());

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
void ZFAdForSplash::_ZFP_ZFAdForSplash_onLoad(void) {
    if(d->loading) {
        d->loading = zffalse;
        if(!d->loadCallbacks.isEmpty()) {
            ZFCoreArray<ZFListener> tmp;
            tmp.swap(d->loadCallbacks);
            ZFArgs zfargs;
            zfargs.sender(this);
            for(zfindex i = 0; i < tmp.count(); ++i) {
                tmp[i].execute(zfargs);
            }
        }
        this->observerNotify(zfself::E_AdOnLoad());
        zfobjRelease(this);
    }
}
void ZFAdForSplash::_ZFP_ZFAdForSplash_stop(
        ZF_IN ZFResultType resultType
        , ZF_IN const zfstring &errorHint
        ) {
    if(d->started) {
        d->stop();
        if(resultType == v_ZFResultType::e_Fail) {
            this->observerNotify(zfself::E_AdOnError(), zfobj<v_zfstring>(errorHint));
        }
        this->observerNotify(zfself::E_AdOnStop(), zfobj<v_ZFResultType>(resultType), zfobj<v_zfstring>(errorHint));
        zfobjRelease(this);
    }
}

ZF_NAMESPACE_GLOBAL_END

