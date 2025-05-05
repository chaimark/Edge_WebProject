import appData from "./AppData.json"

appData.ServerDevTime = "";

//***********************************//
// 新建对象
const xhr = new XMLHttpRequest();
// 设置请求方法
xhr.open('GET', appData.ProductUrl, true);
// 设置请求头，表示发送的是JSON数据
xhr.setRequestHeader('Content-Type', 'application/json;charset=UTF-8');
// 设置发送请求后的回调函数
xhr.onreadystatechange = function () {
	if (xhr.readyState === XMLHttpRequest.DONE) {
		if (xhr.status === 200) {
			appData.ServerDevTime = JSON.parse(xhr.responseText); // 解析响应数据,获取服务器时间
		} else {
			alert("The time to get the ServerDev is Error");
		}
	}
};

//***********************************//
// 目标时间（未来的某个时间点）

// 计算剩余时间
function updateCountdown() {
	const targetDate = new Date(appData.OverTime).getTime();
	const ServerDevnow = new Date(appData.ServerDevTime).getTime();
	const distance = targetDate - ServerDevnow;
	// 计算剩余的时间
	const hours = Math.floor((distance % (1000 * 60 * 60 * 24)) / (1000 * 60 * 60));
	const minutes = Math.floor((distance % (1000 * 60 * 60)) / (1000 * 60));
	const seconds = Math.floor((distance % (1000 * 60)) / 1000);

	return hours * 60 * 60 + minutes * 60 + seconds;
}

// 点击购买函数
function click_class(button_Class) {
	var specificButton = document.querySelector(button_Class);
	if (specificButton) {
		specificButton.click();// 模拟点击该按钮
	}
}

// 如果当前网址与目标网址一致，则开始获取服务器时间
if (appData.ProductUrl == window.location.href) {
	xhr.send();	// 获取服务器时间
	var overplusTime = 0;
	if (appData.ServerDevTime != "") {	// 获取成功
		overplusTime = updateCountdown()	// 计算剩余时间 overplusTime 秒		
	}
	//判断剩余时间小于 10 s 点击购买
	if (overplusTime <= 10) {
		click_class(".adblock-tips");
	}
}
