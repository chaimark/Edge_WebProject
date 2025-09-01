// 检查当前页面是否是 ctwing
function CTwingTask () {
    if (isUserPage("ctwing")) {
        startAutoRefresh(60);
    } else {
        console.log("当前页面不是 ctwing, 不会自动刷新。");
    }
}

