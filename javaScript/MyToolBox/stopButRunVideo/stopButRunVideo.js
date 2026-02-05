////////////////////////////////////////
var videoStopButDown_BiliBili = new _videoStopButDown({
    step: 0.2,
    interval: 400,
    threshold: 1,
    onStateChange: (state) => {
        console.log('缓冲状态变更:', state);
    }
});

function Bilibili_Video_Pro () {
    if (!isUserPage("bilibili")) {
        console.log("当前页面不是" + "bilibili");
        return;
    }
    chrome.runtime.onMessage.addListener(function (request, sender, sendResponse) {
        if (request.action === 'runStopVideoRnuDownScript') {
            sendResponse('run ok.');
            let timer = setTimeout(() => {
                videoStopButDown_BiliBili.StopButDownTask("video.TouchGestureForbidScroll");
                clearTimeout(timer);
            }, 1000);
        }
    });
}

////////////////////////////////////////
var videoStopButDown_Mmkv = new _videoStopButDown({
    step: 0.2,
    interval: 400,
    threshold: 1,
    onStateChange: (state) => {
        console.log('缓冲状态变更:', state);
    }
});

function Mmkv_Video_Pro () {
    if (!isUserPage("mmkv")) {
        console.log("当前页面不是" + "mmkv");
        return;
    }
    chrome.runtime.onMessage.addListener(function (request, sender, sendResponse) {
        if (request.action === 'runStopVideoRnuDownScript') {
            sendResponse('run ok.');
            let timer = setTimeout(() => {
                videoStopButDown_Mmkv.StopButDownTask(".dplayer-video.dplayer-video-current");
                clearTimeout(timer);
            }, 1000);
        }
    });
}

////////////////////////////////////////
var videoStopButDown_2rk = new _videoStopButDown({
    step: 0.2,
    interval: 400,
    threshold: 1,
    onStateChange: (state) => {
        console.log('缓冲状态变更:', state);
    }
});

function TwoRK_Video_Pro () {
    if (!isUserPage("2rk")) {
        console.log("当前页面不是" + "2rk");
        return;
    }
    chrome.runtime.onMessage.addListener(function (request, sender, sendResponse) {
        if (request.action === 'runStopVideoRnuDownScript') {
            sendResponse('run ok.');
            let timer = setTimeout(() => {
                videoStopButDown_2rk.StopButDownTask('video[style="outline:0"]');
                clearTimeout(timer);
            }, 1000);
        }
    });
}