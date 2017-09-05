//.pragma library

// 登录信息提示
function setMessage(messageText, isError) {
    errorRect.visible = true
    errerInfo.text = messageText;
    //errerInfo.color = isError ? '#ee2c2c' : '#454545';
}


// 账号校验
function validateUserName() {

    var userNameText = userName.text.trim();
    var isValid = true;
    if (userNameText === '') {
        setMessage('用户名不能为空', true);
        shock.running = true
        isValid = false;
    }

    if (!isValid) {
        userName.focus = true;
        userNameText === '' || userName.selectAll()
    }
    return isValid;
}

// 密码校验
function validatePassword() {

    var passWordText = passWord.text.trim();
    var isValid = true;
    if (passWordText === '') {
        setMessage('密码不能为空', true);
        shock.running = true
        isValid = false;
    }

    if (!isValid) {
        passWord.focus = true;
        passWordText === '' || passWord.selectAll()
    }
    return isValid;
}

// 账号和密码校验结合
function validate() {
    return validateUserName() && validatePassword();
}

// 取消登录
function cancelLogin() {
    errorRect.visible = false
    login.state = login.stateNoLogin

}

// 登录
function Login() {
    showUserList = false
    if (validate()) {
        login.state = login.stateLoginning
        saveUserInfo()
        console.log(setting.loginUserInfos)
        setMessage('登录中',true)

    }
}

// 保存登录信息
function saveUserInfo() {

    // 移出相同用户名的用户信息
    loginUserInfos.some(function(userInfo, index) {
        if (userInfo.userName === userName.text) {
            loginUserInfos.splice(index, 1);
            return true;
        }
    });

    setting.loginUserInfos = JSON.stringify([{
                          userName: userName.text,
                          passWord: passWord.text,
                          saveUser: saveUser.checked ,
                          autoLogin: autoLogin.checked ,
                          // 追加时间戳，避免数据不更新
                          timestamp: Date.now()
                      }].concat(loginUserInfos).slice(0, 5)); // 记录最近5条登录信息
}

// 重新加载登录信息
function loadUserInfo(userInfo) {
    userName.text = userInfo.userName;
    passWord.text = userInfo.passWord;
    saveUser.checked = userInfo.saveUser;
    autoLogin.checked = userInfo.autoLogin;
}

// 重新设置登录信息
function resetUserInfo() {
    userName.text = '';
    passWord.text = '';
    saveUser.checked = true;
    autoLogin.checked = false;
    showUserList = false;
}

// 移除某个登录信息
function removeUserInfo(userInfo) {
    loginUserInfos.some(function(currentUserInfo, index) {
        if (currentUserInfo.userName === userInfo.userName) {
            loginUserInfos.splice(index, 1);
            return true;
        }
    });

    setting.loginUserInfos = JSON.stringify(loginUserInfos);

    // 如果当前显示的是删除的用户信息，则重新设置登录框中显示的用户信息
    if (userInfo.userName === userName.text) {

        // 如果当前记录的用户列表中还有用户信息，则加载第一个用户信息，否则重置登录框上的用户信息为默认值
        if (loginUserInfos.length > 0) {
            loadUserInfo(loginUserInfos[0]);
        } else {
            resetUserInfo();
        }
    }
}
