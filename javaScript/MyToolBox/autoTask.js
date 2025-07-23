// 页面加载完成后，延迟 5s, 自动执行
window.onload = function () {
    let timer = setTimeout(function () {
        clearTimeout(timer);
        // CTwingTask();
        // BaiduTask();
        douyinVideoTask();
        Bilibili_Video_Pro();   // bilibili 脚本视频
        Mmkv_Video_Pro();       // mmkv 脚本视频
        xigua_Video_Pro();      // 西瓜动漫视频
    }, 500);
}

