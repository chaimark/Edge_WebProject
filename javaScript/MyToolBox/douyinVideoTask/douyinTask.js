var videoTouchData = new _videoTouchData();
var sliderTouchData = new _videoTouchData();

function douyinVideoTask () {
    videoTouchData.videoTouchTask("douyin", "xg-video-container", "class");
    sliderTouchData.videoTouchTask("douyin", "slider-card", "id");
}

// 监控页面点击事件
document.addEventListener("touchstart", function () {
    videoTouchData.isTouch = true;
    videoTouchData.isTouchStart = true;
    sliderTouchData.isTouch = true;
    sliderTouchData.isTouchStart = true;
});
// 监控页面点击事件
document.addEventListener("touchmove", function () {
    videoTouchData.isTouchMove = true;
    sliderTouchData.isTouchMove = true;
});
// 监控页面点击事件
document.addEventListener("touchend", function () {
    videoTouchData.isTouchEnd = true;
    sliderTouchData.isTouchEnd = true;

    if (videoTouchData.getisRefresh()) {
        videoTouchData.refreshVideo();
    }
    if (sliderTouchData.getisRefresh()) {
        sliderTouchData.refreshVideo();
    }
});

