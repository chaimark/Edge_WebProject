import appData from "./AppData.json"

// 定义要执行的函数
function handleClick() {
    alert('参数设置成功!');
	const jsonData = JSON.parse(appData);
	jsonData.ProductNum = 1;
    // 将修改后的数据转换回JSON格式
    const newData = JSON.stringify(jsonData, null, 2);

    // 写入JSON文件
    fs.writeFile('AppData.json', newData, 'utf8', (err) => {
        if (err) {
            console.error(err);
            return;
        }
        console.log('JSON文件已成功修改！');
    });
}
// 获取按钮元素
const myButton = document.getElementById('myButton');
// 绑定点击事件
myButton.addEventListener('click', handleClick);





