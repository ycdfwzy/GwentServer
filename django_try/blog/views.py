from django.shortcuts import render

# Create your views here.
from django import forms
from django.shortcuts import render_to_response
from django.http import HttpResponse,HttpResponseRedirect
from django.template import RequestContext
from blog.models import User
import json

#定义表单模型
class UserForm(forms.Form):
    username = forms.CharField(label='用户名',max_length=100)
    passworld = forms.CharField(label='密码',widget=forms.PasswordInput())
    #email = forms.EmailField(label='电子邮件')

# Create your views here.
def register(request):
    if request.method == "POST":
        uf = UserForm(request.POST)
        if uf.is_valid():
            #获取表单信息
            username = uf.cleaned_data['username']
            passworld = uf.cleaned_data['passworld']
            #email = uf.cleaned_data['email']
            #do some check
            file = open(r"D:\git\GwentServer\data\users\all.json", "r")
            fileJson = json.load(file)
            file.close()
            for us in fileJson:
                if us["username"] == username:
                    return render_to_response('failure.html', {'username': username})

            #将表单写入数据库
            dic = {"username": username, "password": passworld, "status": "Offline"}
            fileJson.append(dic)
            file = open(r"D:\git\GwentServer\data\users\all.json", "w")
            json.dump(fileJson, file)
            file.close()
            
            cards = [16, 8, 19, 17, 7, 57, 66, 75, 169, 188, 181, 182, 256, 225, 236, 281, 63]
            decks = {"deckname": "stater", "leader": 183, "cards": cards}
            decks = [decks]
            usr = {"name": username, "totalgames": 0, "victorygames": 0, "drawgames": 0, "defeatgames": 0, "decks": decks}
            path_dir = "D:\\git\\GwentServer\\data\\users\\" + username + ".json";
            file = open(path_dir, "w")
            json.dump(usr, file)
            file.close()
            '''
            user = User()
            user.username = username
            user.passworld = passworld
            user.email = email
            user.save()
            '''
            #返回注册成功页面
            return render_to_response('success.html',{'username':username})
    else:
        uf = UserForm()
    return render_to_response('register.html',{'uf':uf})