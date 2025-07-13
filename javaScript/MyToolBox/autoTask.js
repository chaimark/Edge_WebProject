// 页面加载完成后，延迟 5s, 自动执行
window.onload = function () {
    let timer = setTimeout(function () {
        clearTimeout(timer);
        // CTwingTask();
        // BaiduTask();
        // douyinVideoTask();
        Bilibili_Video_Pro();
    }, 500);
}

