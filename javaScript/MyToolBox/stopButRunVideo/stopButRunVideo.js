var videoStopButDown_BiliBili = new _videoStopButDown({
    step: 0.2,
    interval: 400,
    threshold: 1,
    onStateChange: (state) => {
        console.log('缓冲状态变更:', state);
    }
});

function Bilibili_Video_Pro () {
    let timer = setTimeout(() => {
        videoStopButDown_BiliBili.StopButDownTask("TouchGestureForbidScroll");
    }, 3000);

}