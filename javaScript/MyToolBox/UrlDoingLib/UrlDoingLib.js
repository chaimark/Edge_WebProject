// 检查当前页面的 Url 是否包含某字段
function isUserPage (UrlStr) {
    // 获取当前页面的URL
    const currentUrl = window.location.href.toLowerCase();
    return currentUrl.includes(UrlStr);
}

// 开始定时刷新主页，intervalTime 秒
function startAutoRefresh (intervalTimeS) {
    setInterval(function () {
        window.location.reload();
    }, intervalTimeS * 1000);
}

// export {isUserPage, startAutoRefresh};

