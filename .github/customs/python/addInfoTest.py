import time
import subprocess
import sys
import threading
from encodings.utf_8 import encode
from select import select


def myReadLine(obj):
    data = ""
    s = select([obj.fileno()], [], [], 1)
    while s[0]:
        s = select([obj.fileno()], [], [], 2)
        data += obj.readline()
    return data


def literRead(obj):
    data = ""
    t = obj.read(1)
    while t != '#':
        data += t
        t = obj.read(1)
    return data


def file2Str(obj):
    data = ""
    while not obj.read():
        data += obj.readline()


def getOutputElement(obj, aim):
    data = ""
    for i in obj:
        if i.find(aim) != -1:
            data = i
            obj.remove(i)
            break
    data = data.replace(aim, "")
    # 移除多余的意外匹配的内容
    data = data.replace("\r", "")
    data = data.replace(" ", "")
    data = data.replace("\n", "")
    return data


def processOutput(strobj):
    # Q: 此处data是什么类型？
    # A: data是一个列表，有7个元素，每个元素都是一个列表，每个元素的第一个元素是UID，第二个元素是姓名，第三个元素是性别，第四个元素是生日，第五个元素是邮箱
    #   第六个元素是年龄，第七个元素是用户硬币数
    data = [], [], [], [], [], [], []
    splited = strobj.split("$")
    # 用于移除字符串头的空字符串，和防止非标准的末尾$符号产生的空字符串让脚本崩溃
    for i in splited:
        if len(i) == 0:
            splited.remove(i)
        for j in i:
            if len(j) == 0:
                splited.remove(j)
                break
    # 开始处理数据
    for i in splited:
        tmp = i.split("\n")
        data[0].append(getOutputElement(tmp, "UID:"))
        data[1].append(getOutputElement(tmp, "用户名:"))
        data[2].append(getOutputElement(tmp, "性别:"))
        data[3].append(getOutputElement(tmp, "出生日期:"))
        data[4].append(getOutputElement(tmp, "邮箱:"))
        data[5].append(getOutputElement(tmp, "年龄:"))
        data[6].append(getOutputElement(tmp, "硬币:"))
    return data


def testProgramWatchDog():
    time.sleep(3)
    if test_obj.poll() is None:
        test_obj.send_signal(9)
        print("[WARNING] 被测试的程序超时！")


def calculateAge(birthday):
    # 输入为字符串，格式为YYYY-MM-DD
    # 输出为整数，表示年龄
    today = time.strftime("%Y-%m-%d", time.localtime())
    today = today.split("-")
    birthday = birthday.split("-")
    age = int(today[0]) - int(birthday[0])
    if int(today[1]) < int(birthday[1]):
        age -= 1
    elif int(today[1]) == int(birthday[1]):
        if int(today[2]) < int(birthday[2]):
            age -= 1
    # 将年龄转换为字符串
    age = str(age)
    return age


exceptOutput = ["1", "2", "3", "4", "5"], ["testUser1", "testUser2", "testUser3", "testUser4", "testUser5"], \
    ["男", "女", "男", "女", "男"], ["2001-02-03", "2002-03-04", "2003-04-05", "1984-05-06", "2011-06-07"], \
    ["testUser1@cuit.edu.cn", "testUser2@cuit.edu.cn", "testUser3@cuit.edu.cn",
     "testUser4@cuit.edu.cn", "testUser5@cuit.edu.cn"], \
    [calculateAge("2001-02-03"), calculateAge("2002-03-04"), calculateAge("2003-04-05"),
     calculateAge("1984-05-06"), calculateAge("2011-06-07")], \
    ["100", "100", "100", "100", "100"]

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("[ERROR] 脚本调用参数不正确！")
        exit(3)
    print("[INFO] 添加学生测试开始")
    test_obj = subprocess.Popen(sys.argv[1], stdin=subprocess.PIPE, stdout=subprocess.PIPE, universal_newlines=True,
                                stderr=subprocess.STDOUT, shell=True)
    test_input = "1\r\ntestUser1\r\nM\r\n2001-02-03\r\ntestUser1@cuit.edu.cn\r\n"
    test_input += "1\r\ntestUser2\r\nF\r\n2002-03-04\r\ntestUser2@cuit.edu.cn\r\n"
    test_input += "1\r\ntestUser3\r\nM\r\n2003-04-05\r\ntestUser3@cuit.edu.cn\r\n"
    test_input += "1\r\ntestUser4\r\nF\r\n1984-05-06\r\ntestUser4@cuit.edu.cn\r\n"
    test_input += "1\r\ntestUser5\r\nM\r\n2011-06-07\r\ntestUser5@cuit.edu.cn\r\n"
    test_input += "2\r\n"
    test_obj.stdin.write(test_input)
    test_obj.stdin.flush()
    watchdog = threading.Thread(target=testProgramWatchDog)
    watchdog.start()
    try:
        actual_output = test_obj.communicate(timeout=5)
    except subprocess.TimeoutExpired:
        print("[ERROR] 被测试程序无法在规定时间内结束！")
        exit(2)
    test_obj.kill()
    print(actual_output[0])
    actual_output = processOutput(actual_output[0])
    for i in actual_output:
        if i not in exceptOutput:
            print("[ERROR] 被测试的程序输出不正确！")
            place = actual_output.index(i)
            print("[ERROR] 实际输出："+str(i))
            print("[ERROR] 期望输出："+str(exceptOutput[place]))
            exit(1)
    print("[INFO] 测试结束")
    exit(0)
