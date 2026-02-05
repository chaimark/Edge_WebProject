// ReadHtml_Task
import {readNowHtml} from './ReadHtml/ReadHtml.js';
const elementReadCSDN = document.getElementById('ReadCSDN');
if (elementReadCSDN) {
    // 为按钮添加 click 事件监听器
    elementReadCSDN.addEventListener("click", readNowHtml);
}
// OpenMyPage
import {openUserPage} from './OpenMyPage/OpenPage.js';
const elementQuizButton = document.getElementById('QuizButton');
if (elementQuizButton) {
    // 为按钮添加 click 事件监听器
    elementQuizButton.addEventListener("click", openUserPage);
}

// 发送消息到 autoTask
const elementvideoRunButton = document.getElementById('videoRunButton');
if (elementvideoRunButton) {
    // 为按钮添加 click 事件监听器
    elementvideoRunButton.addEventListener("click", function () {
        chrome.tabs.query({active: true, currentWindow: true}, function (tabs) {
            console.log("send ok");
            chrome.tabs.sendMessage(
                tabs[0].id, // 注意：tabs 是数组，要取第一个元素
                {action: 'runStopVideoRnuDownScript'},
                {}, // 可选的 options 参数（可以留空）
                function (response) {
                    console.log('收到响应:', response);
                }
            );
        });
    });
}

// 发送消息到 autoPayTask
const elementTimePayButton = document.getElementById('TimePay');
if (elementTimePayButton) {
    // 为按钮添加 click 事件监听器
    elementTimePayButton.addEventListener("click", function () {
        chrome.tabs.query({active: true, currentWindow: true}, function (tabs) {
            console.log("send ok");
            chrome.tabs.sendMessage(
                tabs[0].id, // 注意：tabs 是数组，要取第一个元素
                {action: 'runTaoBaoTimePayScript'},
                {}, // 可选的 options 参数（可以留空）
                function (response) {
                    console.log('收到响应:', response);
                }
            );
        });
    });
}

