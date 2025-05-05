// ReadHtml_Task
import {readNowHtml} from './ReadHtml/ReadHtml.js';
const elementReadCSDN = document.getElementById('ReadCSDN');
if (elementReadCSDN) {
    // 为按钮添加 click 事件监听器
    document.getElementById("ReadCSDN").addEventListener("click", readNowHtml);
}
// OpenMyPage
import {openUserPage} from './OpenMyPage/OpenPage.js';
const elementQuizButton = document.getElementById('QuizButton');
if (elementQuizButton) {
    // 为按钮添加 click 事件监听器
    document.getElementById("QuizButton").addEventListener("click", openUserPage);
}
