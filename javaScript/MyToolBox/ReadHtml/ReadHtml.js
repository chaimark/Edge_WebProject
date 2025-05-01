function readNowHtml () {
    // 删除 passportbox 元素
    setTimeout(function () {
        var passportbox = document.querySelector('.passportbox');
        if (passportbox) {
            passportbox.remove();
        }

        chrome.tabs.query({active: true, currentWindow: true}, function (tabs) {
            // 获取当前选项卡的URL
            let urlOfNeedOpen = 'read://' + tabs[0].url;
            chrome.tabs.update(tabs[0].id, {url: urlOfNeedOpen});
        });
    }, 100);
}

export {readNowHtml};