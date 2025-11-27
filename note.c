#include <stdio.h>
#include <string.h>

#define MAX_CONTACTS 200
#define NAME_LEN 32
#define PHONE_LEN 20
#define NOTE_LEN 64
#define LINE_LEN 256

typedef struct {
    char name[NAME_LEN];
    char phone[PHONE_LEN];
    char note[NOTE_LEN];
} Contact;

Contact book[MAX_CONTACTS];
int count = 0;

/* 去掉 fgets 读入的末尾换行 */
void trim_newline(char *s) {
    size_t n = strlen(s);
    if (n > 0 && s[n - 1] == '\n') s[n - 1] = '\0';
}

/* 安全读取一行 */
void safe_readline(const char *prompt, char *buf, int len) {
    printf("%s", prompt);
    if (fgets(buf, len, stdin)) {
        trim_newline(buf);
    } else {
        // 读失败就置空
        buf[0] = '\0';
    }
}

/* 新增联系人 */
void add_contact() {
    if (count >= MAX_CONTACTS) {
        printf("通讯录已满，无法新增。\n");
        return;
    }
    char name[NAME_LEN], phone[PHONE_LEN], note[NOTE_LEN];

    safe_readline("请输入姓名: ", name, NAME_LEN);
    safe_readline("请输入手机号: ", phone, PHONE_LEN);
    safe_readline("请输入备注: ", note, NOTE_LEN);

    // 使用字符串拷贝函数
    strncpy(book[count].name, name, NAME_LEN - 1);
    book[count].name[NAME_LEN - 1] = '\0';

    strncpy(book[count].phone, phone, PHONE_LEN - 1);
    book[count].phone[PHONE_LEN - 1] = '\0';

    strncpy(book[count].note, note, NOTE_LEN - 1);
    book[count].note[NOTE_LEN - 1] = '\0';

    count++;
    printf("新增成功！当前联系人数量: %d\n", count);
}

/* 按姓名精确删除 */
void delete_contact_by_name() {
    if (count == 0) {
        printf("通讯录为空。\n");
        return;
    }
    char target[NAME_LEN];
    safe_readline("请输入要删除的姓名(精确): ", target, NAME_LEN);

    int idx = -1;
    for (int i = 0; i < count; i++) {
        if (strcmp(book[i].name, target) == 0) { // 字符串比较
            idx = i;
            break;
        }
    }
    if (idx == -1) {
        printf("未找到该联系人。\n");
        return;
    }

    for (int i = idx; i < count - 1; i++) {
        book[i] = book[i + 1]; // 结构体整体移动
    }
    count--;
    printf("删除成功！当前联系人数量: %d\n", count);
}

/* 按姓名精确查找 */
void find_contact_exact() {
    if (count == 0) {
        printf("通讯录为空。\n");
        return;
    }
    char target[NAME_LEN];
    safe_readline("请输入要查找的姓名(精确): ", target, NAME_LEN);

    for (int i = 0; i < count; i++) {
        if (strcmp(book[i].name, target) == 0) {
            printf("找到联系人:\n");
            printf("姓名: %s | 手机: %s | 备注: %s\n",
                   book[i].name, book[i].phone, book[i].note);
            return;
        }
    }
    printf("未找到该联系人。\n");
}

/* 模糊查找：姓名或手机号包含关键字 */
void find_contact_fuzzy() {
    if (count == 0) {
        printf("通讯录为空。\n");
        return;
    }
    char key[NAME_LEN];
    safe_readline("请输入关键字(姓名/手机号子串): ", key, NAME_LEN);

    int found = 0;
    for (int i = 0; i < count; i++) {
        // strstr 用于子串匹配
        if (strstr(book[i].name, key) || strstr(book[i].phone, key)) {
            printf("姓名: %s | 手机: %s | 备注: %s\n",
                   book[i].name, book[i].phone, book[i].note);
            found = 1;
        }
    }
    if (!found) printf("没有匹配结果。\n");
}

/* 按姓名字典序排序后显示 */
void list_contacts_sorted() {
    if (count == 0) {
        printf("通讯录为空。\n");
        return;
    }

    // 复制一份用于排序显示，避免改变原顺序
    Contact temp[MAX_CONTACTS];
    for (int i = 0; i < count; i++) temp[i] = book[i];

    // 冒泡排序，strcmp 控制字典序
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - 1 - i; j++) {
            if (strcmp(temp[j].name, temp[j + 1].name) > 0) {
                Contact t = temp[j];
                temp[j] = temp[j + 1];
                temp[j + 1] = t;
            }
        }
    }

    printf("---- 全部联系人(按姓名排序) ----\n");
    for (int i = 0; i < count; i++) {
        printf("%2d) 姓名: %-10s 手机: %-15s 备注: %s\n",
               i + 1, temp[i].name, temp[i].phone, temp[i].note);
    }
}

/* 导出 CSV */
void export_csv() {
    FILE *fp = fopen("contacts.csv", "w");
    if (!fp) {
        printf("无法创建 contacts.csv\n");
        return;
    }

    for (int i = 0; i < count; i++) {
        // snprintf 属于安全的字符串拼接/格式化
        fprintf(fp, "%s,%s,%s\n", book[i].name, book[i].phone, book[i].note);
    }
    fclose(fp);
    printf("导出成功：contacts.csv\n");
}

/* 导入 CSV（覆盖当前通讯录） */
void import_csv() {
    FILE *fp = fopen("contacts.csv", "r");
    if (!fp) {
        printf("未找到 contacts.csv\n");
        return;
    }

    count = 0;
    char line[LINE_LEN];

    while (fgets(line, LINE_LEN, fp)) {
        trim_newline(line);
        if (strlen(line) == 0) continue;

        // strtok 分割 CSV
        char *name = strtok(line, ",");
        char *phone = strtok(NULL, ",");
        char *note = strtok(NULL, ",");

        if (!name || !phone) continue;

        strncpy(book[count].name, name, NAME_LEN - 1);
        book[count].name[NAME_LEN - 1] = '\0';

        strncpy(book[count].phone, phone, PHONE_LEN - 1);
        book[count].phone[PHONE_LEN - 1] = '\0';

        if (note) {
            strncpy(book[count].note, note, NOTE_LEN - 1);
            book[count].note[NOTE_LEN - 1] = '\0';
        } else {
            book[count].note[0] = '\0';
        }

        count++;
        if (count >= MAX_CONTACTS) break;
    }

    fclose(fp);
    printf("导入完成：当前联系人数量 %d\n", count);
}

/* 菜单 */
void menu() {
    printf("\n========= 简易通讯录 =========\n");
    printf("1. 新增联系人\n");
    printf("2. 删除联系人(按姓名)\n");
    printf("3. 精确查找(按姓名)\n");
    printf("4. 模糊查找(姓名/手机号)\n");
    printf("5. 显示全部联系人(姓名排序)\n");
    printf("6. 导出CSV\n");
    printf("7. 导入CSV\n");
    printf("0. 退出\n");
    printf("请选择: ");
}

int main() {
    int choice;

    while (1) {
        menu();
        if (scanf("%d", &choice) != 1) {
            // 输入非数字
            printf("输入无效。\n");
            // 清空输入缓冲
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }
        // 吃掉 scanf 留下的换行
        getchar();

        switch (choice) {
            case 1: add_contact(); break;
            case 2: delete_contact_by_name(); break;
            case 3: find_contact_exact(); break;
            case 4: find_contact_fuzzy(); break;
            case 5: list_contacts_sorted(); break;
            case 6: export_csv(); break;
            case 7: import_csv(); break;
            case 0: printf("再见！\n"); return 0;
            default: printf("无效选项。\n");
        }
    }
    return 0;
}
