var videoStopButDown_BiliBili = new _videoStopButDown({
    step: 0.2,
    interval: 400,
    threshold: 1,
    onStateChange: (state) => {
        console.log('缓冲状态变更:', state);
    }
});

function Bilibili_Video_Pro () {
    let isRun = false;
    if (isRun) return;
    chrome.runtime.onMessage.addListener(function (request, sender, sendResponse) {
        if (request.action === 'runStopVideoRnuDownScript') {
            sendResponse('run ok.');
            let timer = setTimeout(() => {
                videoStopButDown_BiliBili.StopButDownTask("TouchGestureForbidScroll");
                clearTimeout(timer);
                isRun = true;
            }, 1000);
        }
    });
}