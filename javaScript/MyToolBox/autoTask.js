// 页面加载完成后，延迟 5s, 自动执行
window.onload = function () {
    let timer = setTimeout(function () {
        clearTimeout(timer);
        // CTwingTask();
        // BaiduTask();
        // douyinVideoTask();
        // Bilibili_Video_Pro();   // bilibili 脚本视频
        // Mmkv_Video_Pro();       // mmkv 脚本视频
        // TwoRK_Video_Pro();      // 2rk视频
        TimePay_wait();
    }, 500);
}

