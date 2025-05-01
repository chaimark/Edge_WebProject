function openUserPage () {
    var usePageUrl = chrome.runtime.getURL('./OpenMyPage/UserPage.html');
    window.open(usePageUrl, '_blank');
}

export {openUserPage};
