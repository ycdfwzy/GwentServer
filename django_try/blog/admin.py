from django.contrib import admin

# Register your models here.
'''
class UserProfileInline(admin.StackedInline):
    model = UserProfile
    fk_name = 'user'
    max_num = 1


class UserProfileAdmin(UserAdmin):
    inlines = [UserProfileInline, ]


admin.site.unregister(User)
admin.site.register(User, UserProfileAdmin)'''