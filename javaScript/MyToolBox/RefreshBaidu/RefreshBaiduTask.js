// import { isUserPage } from '../UrlDoingLib/UrlDoingLib.js';
// import { startAutoRefresh } from '../UrlDoingLib/UrlDoingLib.js';
// 检查当前页面是否是 baidu
function BaiduTask () {
    if (isUserPage("baidu")) {
        startAutoRefresh(5);
    } else {
        console.log("当前页面不是 baidu, 不会自动刷新。");
    }
}

// export {BaiduTask};