var TaoBaoSetTimeAutoPay = new _autoPayClass();
function TimePay_wait () {
    if (!isUserPage("cart.taobao")) {
        console.log("当前页面不是" + "cart.taobao");
        if (!isUserPage("buy.tmall")) {
            console.log("当前页面不是" + "cart.taobao");
            return;
        } else {
            let timer = setTimeout(() => {
                const submitButton = document.querySelector('.btn--QDjHtErD.btn_disabled--kp_s_bi2');
                if (submitButton) {
                    clearTimeout(timer);
                    console.log("找到提交按钮");
                    submitButton.click();
                } else {
                    console.log("未找到提交按钮");
                }
            }, 500);
        }
    }
    chrome.runtime.onMessage.addListener(function (request, sender, sendResponse) {
        if (request.action === 'runTaoBaoTimePayScript') {
            sendResponse('run ok.');
            let timer = setTimeout(() => {
                TaoBaoSetTimeAutoPay.Set_ButtonAndRunTask(".btn--QDjHtErD");
                clearTimeout(timer);
            }, 1000);
        }
    });
}