function CTwingTask () {
    if (isUserPage("ctwing")) {
        startAutoRefresh(60);
    } else {
        console.log("当前页面不是 ctwing, 不会自动刷新。");
    }
}

