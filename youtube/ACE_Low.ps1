#检查是否以管理员身份运?
if (-NOT ([Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole] "Administrator")) {
    # 重新启动脚本为管理员模式
    Start-Process powershell -Verb runAs -ArgumentList "-File `"$PSCommandPath`""
    exit
}

Write-Host "已获取管理员权限"

function ShowProcess {
    param (
        [int]$NextLen,
        [int]$MaxLen,
        [int]$NowLen = 0 # 默认NowLen为0，如果之前已经处理过，则应该传入当前长度
    )
    $NeedLen = if ($NextLen -le $MaxLen) {
        # 计算需要的长度
        $NextLen - $NowLen
    }
    else {
        $MaxLen - $NowLen
    }
    for ($i = 1; $i -le $NeedLen; $i++) {
        # 使用循环来显示进度条
        Write-Host "■" -NoNewline
        Start-Sleep -Milliseconds 50 # 类似于ping /n 1 127.0.0.1>nul的延时效果
    }
    $NowLen += $NeedLen  # 更新当前长度
    if ($NowLen -eq $MaxLen) {
        # 如果达到最大长度，则重置当前长度
        Write-Host "" # 输出一个换行符
        $NowLen = 0
    }
    $NowLen  # 返回更新后的当前长度（如果需要的话）
}

$MaxLen = 10
$NowLen = 0
$NextLen += 10;
$NowLen = ShowProcess -NextLen $NextLen -MaxLen $MaxLen -NowLen $NowLen
if ($NextLen -ge $MaxLen) {
	$NextLen = 0
}

Get-CimInstance -ClassName Win32_Process -Filter 'name = "SGuard64.exe"' | ForEach-Object {Invoke-CimMethod -InputObject $_ -MethodName SetPriority -Arguments @{Priority = 64}}
$ACEpid = (Get-CimInstance -ClassName Win32_Process -Filter 'name = "SGuard64.exe"').ProcessId
$process = Get-Process -Id $ACEpid
$process.ProcessorAffinity = 134217728

Get-CimInstance -ClassName Win32_Process -Filter 'name = "SGuardSvc64.exe"' | ForEach-Object {Invoke-CimMethod -InputObject $_ -MethodName SetPriority -Arguments @{Priority = 64}}
$ACEpid = (Get-CimInstance -ClassName Win32_Process -Filter 'name = "SGuardSvc64.exe"').ProcessId
$process = Get-Process -Id $ACEpid
$process.ProcessorAffinity = 134217728

timeout -t 3
