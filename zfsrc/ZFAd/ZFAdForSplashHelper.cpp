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
    zfautoT<ZFUIWindow> loadingViewWindow;
    zfautoT<ZFTimer> loadingViewWindowHideDelay;
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
    , loadingViewWindow()
    , loadingViewWindowHideDelay()
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

    if(this->skipCount() > 0) {
        zfindex count = zfindexMax();
        zfstring countStr = ZFState::instance()->get("ZFAdForSplashHelper_skipCount");
        if(countStr) {
            zfindexFromStringT(count, countStr);
        }
        if(count < this->skipCount()) {
            return;
        }
    }

    d->holder = this;
    d->loadingViewWindow = zfobj<ZFUIWindow>(this->window());
    d->loadingViewWindow->viewId("ZFAdForSplashLoadingView");
    d->loadingViewWindow->bgColor(ZFUIColorCreateRGB(0x000000));
    d->loadingViewWindow->windowLevel(ZFUIWindowLevelOverlay() - 1);
    d->loadingViewWindow->windowMarginShouldApply(zffalse);
    if(this->loadingView()) {
        this->loadingView()->removeFromParent();
        d->loadingViewWindow->child(this->loadingView())->sizeFill();
    }
    d->loadingViewWindow->show();

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

                if(weakOwner->d->loadingViewWindowHideDelay) {
                    weakOwner->d->loadingViewWindowHideDelay->stop();
                }
                ZFLISTENER_1(loadingViewWindowOnDelayHide
                        , zfweakT<ZFAdForSplashHelper>, weakOwner
                        ) {
                    if(weakOwner->d->loadingViewWindow) {
                        weakOwner->d->loadingViewWindow->hide();
                        weakOwner->d->loadingViewWindow = zfnull;
                    }
                } ZFLISTENER_END()
                weakOwner->d->loadingViewWindowHideDelay = ZFTimerOnce(200, loadingViewWindowOnDelayHide);

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
            if(owner->d->loadingViewWindowHideDelay) {
                owner->d->loadingViewWindowHideDelay->stop();
                owner->d->loadingViewWindowHideDelay = zfnull;
            }
            if(owner->d->loadingViewWindow) {
                owner->d->loadingViewWindow->hide();
                owner->d->loadingViewWindow = zfnull;
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

    {
        zfindex count = zfindexMax();
        zfstring countStr = ZFState::instance()->get("ZFAdForSplashHelper_skipCount");
        if(countStr) {
            zfindexFromStringT(count, countStr);
        }
        ZFState::instance()->set("ZFAdForSplashHelper_skipCount", zfindexToString(++count));
    }

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

// ============================================================
ZFMETHOD_DEFINE_2(ZFAdForSplashHelper, void, loadingIcon
        , ZFMP_IN(ZFUIImage *, icon)
        , ZFMP_IN_OPT(ZFUIColor, bgColor, ZFUIColorCreateRGB(0x000000))
        ) {
    zfobj<ZFUIView> container;
    container->bgColor(bgColor);
    this->loadingView(container);

    zfobj<ZFUIImageView> view;
    container->child(view)->alignCenter();
    view->image(icon);

    zfobj<ZFAniQueue> ani;
    ZFLISTENER_1(onUpdate
            , zfautoT<ZFAnimation>, ani
            ) {
        ZFUIView *owner = zfargs.sender();
        if(owner->viewTreeInWindow()) {
            if(!ani->started()) {
                ani->target(owner);
                ani->start();
            }
        }
        else {
            if(ani->started()) {
                ani->stop();
            }
        }
    } ZFLISTENER_END()
    view->observerAdd(ZFUIView::E_ViewTreeInWindowOnUpdate(), onUpdate);

    zftimet dOffset = 800;
    zftimet dScale = 80;
    zffloat offset = 48;
    zffloat scale = 0.2f;
    ani->loop(zfindexMax());
    ani
        ->c_child(ZFAni("translateY", zfobj<v_zffloat>(0), zfobj<v_zffloat>(-offset))->c_curve(zfobj<ZFCurveEaseOut>())->c_duration(dOffset))
        ->c_child(ZFAni("translateY", zfobj<v_zffloat>(-offset), zfobj<v_zffloat>(0))->c_curve(zfobj<ZFCurveEaseIn>())->c_duration(dOffset))
        ->c_child(zfobj<ZFAniGroup>()
                ->c_child(ZFAni("scaleX", zfobj<v_zffloat>(1), zfobj<v_zffloat>(1 + scale)))
                ->c_child(ZFAni("scaleY", zfobj<v_zffloat>(1), zfobj<v_zffloat>(1 - scale)))
                ->c_duration(dScale)
                )
        ->c_child(zfobj<ZFAniGroup>()
                ->c_child(ZFAni("scaleX", zfobj<v_zffloat>(1 + scale), zfobj<v_zffloat>(1)))
                ->c_child(ZFAni("scaleY", zfobj<v_zffloat>(1 - scale), zfobj<v_zffloat>(1)))
                ->c_duration(dScale)
                )
        ;
}

// ============================================================
ZFMETHOD_FUNC_DEFINE_1(zfautoT<ZFTask>, ZFAdForSplashTask
        , ZFMP_IN(ZFAdForSplashHelper *, ad)
        ) {
    zfobj<ZFObject> observerHolder;

    ZFLISTENER_2(onStart
            , zfauto, observerHolder
            , zfautoT<ZFAdForSplashHelper>, ad
            ) {
        ZFTask *ownerTask = zfargs.sender();
        ad->attach();
        if(ad->window()->windowResumed() && !ad->started()) {
            ownerTask->notifySuccess();
            return;
        }
        ZFLISTENER_1(adOnStop
                , zfweakT<ZFTask>, ownerTask
                ) {
            if(ownerTask) {
                ownerTask->notifySuccess();
            }
        } ZFLISTENER_END()
        ZFObserverGroup(observerHolder, ad)
            .observerAdd(ZFAdForSplash::E_AdOnDisplay(), adOnStop)
            .observerAdd(ZFAdForSplash::E_AdOnError(), adOnStop)
            .observerAdd(ZFAdForSplash::E_AdOnStop(), adOnStop)
            ;

        ZFTimerOnce(3000, adOnStop);
    } ZFLISTENER_END()

    ZFLISTENER_1(onStop
            , zfauto, observerHolder
            ) {
        ZFObserverGroupRemove(observerHolder);
    } ZFLISTENER_END()

    zfobj<ZFTask> task;
    task->observerAdd(ZFTask::E_TaskOnStart(), onStart);
    task->observerAdd(ZFTask::E_TaskOnStop(), onStop);
    return task;
}

ZF_NAMESPACE_GLOBAL_END

