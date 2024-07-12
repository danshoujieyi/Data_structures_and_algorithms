/*

 *
 */
//****************************头文件******************************************************************************//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
//*******************************************************************************************************************//

//*********************************函数结构体与宏定义******************************************************************************//
#define MAX_ACCOUNTS 10 // 最大账号数量
#define NUMBER       30 // 数字替换宏
#define ACCOUNTS_FILE "C:\\Users\\liujiajun\\Desktop\\shujujiegoukeshe\\CLion_C23\\accounts.txt" // 绝对路径 账号文件
#define BOOKS_FILE "C:\\Users\\liujiajun\\Desktop\\shujujiegoukeshe\\CLion_C23\\books.txt" // 绝对路径 书籍文件

typedef struct {
    char username[NUMBER];
    char password[NUMBER];
} Account;

// 存储多个账号的数组
Account accounts[MAX_ACCOUNTS];
int numAccounts = 0; // 将0号设置成管理员账号，0位不允许再创建账号
//原始管理员账号用户名 11
//原始管理员账号密码   22

// 定义图书结构体，包含书籍的基本信息
typedef struct Book {
    int id;              // 图书序号
    char name[NUMBER];       // 图书名称
    char category[NUMBER];   // 所属分类
    char status[NUMBER];     // 借阅状态
    float readTime;      // 阅读耗时
    struct Book* next;   // 指向下一本书的指针
    struct Book* prev;
} Book;

// 定义图书链表结构体，包含链表的头指针和书籍数量
typedef struct {
    int count;
    Book* head;
} BookList;

// 定义分类书籍哈希表
typedef struct {
    Book* books[100]; // 假设每个分类最多100本书
    int size;
} CategoryBooks;

void saveBooks(BookList* list); // 保存书籍信息到文本文件books.txt函数头
//***********************************************************************************************************************************//

//*********************************算法功能函数**********************************************************************************************//
// 快速排序算法
// 交换两个书籍节点的内容
void swapBooks(Book* book1, Book* book2) {
    int tempId = book1->id;
    char tempName[NUMBER];
    char tempCategory[NUMBER];
    char tempStatus[NUMBER];
    float tempReadTime = book1->readTime;

    strcpy(tempName, book1->name);
    strcpy(tempCategory, book1->category);
    strcpy(tempStatus, book1->status);

    book1->id = book2->id;
    book1->readTime = book2->readTime;
    strcpy(book1->name, book2->name);
    strcpy(book1->category, book2->category);
    strcpy(book1->status, book2->status);

    book2->id = tempId;
    book2->readTime = tempReadTime;
    strcpy(book2->name, tempName);
    strcpy(book2->category, tempCategory);
    strcpy(book2->status, tempStatus);
}
// 分区函数，用于快速排序
Book* partition(Book* low, Book* high) {
    int pivot = high->id;
    Book* i = low->prev;

    for (Book* j = low; j != high; j = j->next) {
        if (j->id <= pivot) {
            i = (i == NULL) ? low : i->next;
            swapBooks(i, j);
        }
    }
    i = (i == NULL) ? low : i->next;
    swapBooks(i, high);
    return i;
}
// 快速排序的递归实现函数
void quickSortUtil(Book* low, Book* high) {
    if (high != NULL && low != high && low != high->next) {
        Book* pivot = partition(low, high);
        quickSortUtil(low, pivot->prev);
        quickSortUtil(pivot->next, high);
    }
}
// 快速排序算法
void quickSort(BookList* list) {
    if (list->count <= 1) return;
    Book* last = list->head;
    while (last->next != NULL) {
        last = last->next;
    }
    quickSortUtil(list->head, last);
    printf("书籍使用快速排序算法已按序号排序。\n");
}

// 生成分类书籍哈希表
void generateCategoryBooksHash(BookList* list, CategoryBooks* categoryBooksHash, const char* categories[], int nCategories) {
    for (int i = 0; i < nCategories; i++) {
        categoryBooksHash[i].size = 0;
    }

    Book* current = list->head;
    while (current != NULL) {
        for (int j = 0; j < nCategories; j++) {
            if (strcmp(current->category, categories[j]) == 0 && strcmp(current->status, "在馆") == 0) {
                categoryBooksHash[j].books[categoryBooksHash[j].size++] = current;
                break;
            }
        }
        current = current->next;
    }

    // 对每个分类的书籍按阅读时间排序
    for (int i = 0; i < nCategories; i++) {
        for (int j = 0; j < categoryBooksHash[i].size - 1; j++) {
            for (int k = 0; k < categoryBooksHash[i].size - j - 1; k++) {
                if (categoryBooksHash[i].books[k]->readTime < categoryBooksHash[i].books[k + 1]->readTime) {
                    Book* temp = categoryBooksHash[i].books[k];
                    categoryBooksHash[i].books[k] = categoryBooksHash[i].books[k + 1];
                    categoryBooksHash[i].books[k + 1] = temp;
                }
            }
        }
    }
}

// 从哈希表中查找并移除特定分类的书籍
Book* findAndRemoveBookByCategory(CategoryBooks* categoryBooksHash, const char* category, int* selectedBooks, const char* categories[], int nCategories) {
    for (int i = 0; i < nCategories; i++) {
        if (strcmp(categories[i], category) == 0) {
            for (int j = 0; j < categoryBooksHash[i].size; j++) {
                Book* book = categoryBooksHash[i].books[j];
                if (selectedBooks[book->id - 1] == 0) {
                    selectedBooks[book->id - 1] = 1;
                    // 移除书籍
                    for (int k = j; k < categoryBooksHash[i].size - 1; k++) {
                        categoryBooksHash[i].books[k] = categoryBooksHash[i].books[k + 1];
                    }
                    categoryBooksHash[i].size--;
                    return book;
                }
            }
        }
    }
    return NULL;
}
//********************************************************************************************************************************************//

//**************************************书籍相关操作功能函数*******************************************************************************/
// 初始化图书链表函数
void initList(BookList* list) {
    list->count = 0;
    list->head = NULL;
}

// 添加书籍到链表函数
void addBook(BookList* list, int id, const char* name, const char* category, const char* status, float readTime) {
    Book* newBook = (Book*)malloc(sizeof(Book));
    if (newBook == NULL) {
        printf("内存分配失败\n");
        return;
    }
    newBook->id = id;
    strcpy(newBook->name, name);
    strcpy(newBook->category, category);
    strcpy(newBook->status, status);
    newBook->readTime = readTime;
    newBook->next = list->head;
    newBook->prev = NULL;

    if (list->head != NULL) {
        list->head->prev = newBook;  // 如果链表不为空，更新原头节点的 prev 指针
    }

    list->head = newBook;
    list->count++;
    printf("您已成功添加书籍：%s; 序号：%d; 分类：%s; 状态：%s; 阅读耗时：%.1f天\n", newBook->name, id, newBook->category, newBook->status,newBook->readTime);
    saveBooks(list); // 自动保存书籍信息
}

// 查找指定序号的书籍
Book* findBookById(BookList* list, int id) {
    Book* curr = list->head;
    // 找到指定序号的书籍节点
    while (curr != NULL && curr->id != id) {
        curr = curr->next;
    }
    return curr;  // 返回找到的书籍节点，如果未找到则返回NULL
}

// 查找指定书名的书籍
Book* findBookByName(BookList* list, const char* name) {
    Book* curr = list->head;
    // 找到指定书名的书籍节点
    while (curr != NULL && strcmp(curr->name, name) != 0) {
        curr = curr->next;
    }
    return curr;  // 返回找到的书籍节点，如果未找到则返回NULL
}

// 按分类显示所有书籍信息
void showBooksByAllCategories(BookList* list) {
    const char* categories[] = {"教科书", "历史", "经济", "文学", "哲学", "地理", "数学", "技术", "医学", "艺术", "宗教", "农学", "管理", "儿童"};
    int numCategories = sizeof(categories) / sizeof(categories[0]);

    for (int i = 0; i < numCategories; i++) {
        const char* category = categories[i];
        Book* current = list->head;
        bool found = false;
        printf("分类为 %s 的书籍列表:\n", category);
        while (current != NULL) {
            if (strcmp(current->category, category) == 0) {
                printf("序号：%d, 书名：%s, 分类：%s, 状态：%s, 阅读耗时：%.1f天\n", current->id, current->name, current->category, current->status, current->readTime);
                found = true;
            }
            current = current->next;
        }
        if (!found) {
            printf("未找到分类为 %s 的书籍。\n", category);
        }
        printf("\n"); // 每个分类之间空一行
    }
    printf("按任意键返回。。。\n");
    getch();
}

// 按在馆状态显示所有书籍信息
void showBooksByStatus(BookList* list) {
    const char* statuses[] = {"在馆", "在借"};
    int numStatuses = sizeof(statuses) / sizeof(statuses[0]);

    for (int i = 0; i < numStatuses; i++) {
        const char* status = statuses[i];
        Book* current = list->head;
        bool found = false;
        printf("状态为 %s 的书籍列表:\n", status);
        while (current != NULL) {
            if (strcmp(current->status, status) == 0) {
                printf("序号：%d, 书名：%s, 分类：%s, 状态：%s, 阅读耗时：%.1f天\n", current->id, current->name, current->category, current->status, current->readTime);
                found = true;
            }
            current = current->next;
        }
        if (!found) {
            printf("未找到状态为 %s 的书籍。\n", status);
        }
        printf("\n"); // 每个状态之间空一行
    }
    printf("按任意键返回。。。\n");
    getch();
}

// 按序号打印所有书籍信息
void printBooks(BookList* list) {
    if (list->count == 0) {
        printf("当前没有任何书籍。\n");
        return;
    }
    Book* curr = list->head;
    while (curr != NULL) {
        printf("序号: %d, 书名: %s, 分类: %s, 状态: %s, 阅读耗时: %.1f 天\n",
               curr->id, curr->name, curr->category, curr->status, curr->readTime);
        curr = curr->next;
    }
    printf("按任意键返回。。。\n");
    getch();
}

// 删除书籍，可以根据书籍序号或书名，如不知道序号输入”0“，不知道书名输入”NULL“
void deleteBook(BookList* list, int id, const char* name) {
    Book* prev = NULL;
    Book* curr = list->head;
    Book* book = NULL;
    // 优先根据序号查找
    if (id > 0) {
        book = findBookById(list, id);
    }
    // 如果序号未找到或无效，使用书名查找
    if (book == NULL && name != NULL) {
        book = findBookByName(list, name);
    }
    // 删除书籍
    if (book != NULL) {
        curr = list->head;
        while (curr != NULL && curr != book) {
            prev = curr;
            curr = curr->next;
        }

        if (prev == NULL) {
            list->head = curr->next;  // 如果是头节点，则更新头指针
        } else {
            prev->next = curr->next;  // 否则更新前一个节点的next指针
        }

        list->count--;
        printf("您已成功删除书籍：%s; 序号：%d; 系统内书籍数量：%d \n", curr->name, curr->id, list->count);
        saveBooks(list); // 自动保存书籍信息
        free(curr);  // 释放内存
        printf("按任意键返回。。。\n");
        getch();
    } else {
        if (id > 0) {
            printf("没有找到序号为 %d 的书籍。\n", id);
        } else if (name != NULL) {
            printf("没有找到书名为 “%s” 的书籍。\n", name);
        }
        printf("按任意键返回。。。\n");
        getch();
    }
}

// 删除所有书籍
void deleteAllBooks(BookList* list) {
    Book* curr = list->head;
    while (curr != NULL) {
        Book* next = curr->next;
        free(curr);
        curr = next;
    }
    list->head = NULL;
    list->count = 0;
    printf("所有书籍已删除，系统内书籍数量：%d\n", list->count);
    saveBooks(list); // 自动保存书籍信息
}

// 更新书籍的借阅状态，可以根据书籍序号或书名，如不知道序号输入”0“，不知道书名输入”NULL“
void updateBookStatus(BookList* list, int id, const char* name, const char* newStatus) {
    Book* book = NULL;
    // 优先根据序号查找
    if (id > 0) {
        book = findBookById(list, id);
    }
    // 如果序号未找到或无效，使用书名查找
    if (book == NULL && name != NULL) {
        book = findBookByName(list, name);
    }
    // 更新书籍状态
    if (book != NULL) {
        char oldStatus[NUMBER];
        strcpy(oldStatus, book->status);  // 保存原状态
        strcpy(book->status, newStatus);  // 更新为新状态
        printf("您已成功更新书籍状态：%s; 序号：%d; 原状态：%s; 新状态为：%s\n", book->name, book->id, oldStatus, newStatus);
        saveBooks(list); // 自动保存书籍信息
        printf("按任意键返回。。。\n");
        getch();
    } else {
        if (id > 0) {
            printf("没有找到序号为 %d 的书籍。\n", id);
        } else if (name != NULL) {
            printf("没有找到书名为 “%s” 的书籍。\n", name);
        }
        printf("按任意键返回。。。\n");
        getch();
    }
}

// 使用序号或书名更新书籍信息，包含书籍序号、名称、分类、在馆状态、阅读耗时
void updateBook(BookList* list) {
    int id;
    char name[NUMBER];
    char newName[NUMBER];
    char newCategory[NUMBER];
    char newStatus[NUMBER];
    float newReadTime;
    int newId;

    printf("请输入要更新的书籍的序号（不知道序号输入0）：");
    scanf("%d", &id);
    getchar();  // 清除输入缓冲区中的换行符

    if (id == 0) {
        printf("请输入要更新的书籍的书名：");
        fgets(name, sizeof(name), stdin);
        name[strcspn(name, "\n")] = '\0';  // 移除换行符
    } else {
        name[0] = '\0';  // 清空书名
    }

    printf("请输入新的序号：");
    scanf("%d", &newId);
    getchar();  // 清除输入缓冲区中的换行符

    printf("请输入新的书名：");
    fgets(newName, sizeof(newName), stdin);
    newName[strcspn(newName, "\n")] = '\0'; // 移除换行符
    printf("请输入新的分类：");
    fgets(newCategory, sizeof(newCategory), stdin);
    newCategory[strcspn(newCategory, "\n")] = '\0'; // 移除换行符
    printf("请输入新的状态：");
    fgets(newStatus, sizeof(newStatus), stdin);
    newStatus[strcspn(newStatus, "\n")] = '\0';
    printf("请输入新的阅读耗时（天）：");
    scanf("%f", &newReadTime);

    Book* bookById = findBookById(list, id);
    Book* bookByName = findBookByName(list, name);
    Book* bookToUpdate = NULL;

    if (bookById != NULL) {
        bookToUpdate = bookById;
    } else if (bookByName != NULL) {
        bookToUpdate = bookByName;
    }

    if (bookToUpdate != NULL) {
        // 检查新序号是否被使用
        if (newId != bookToUpdate->id && findBookById(list, newId) != NULL) {
            printf("新序号 %d 已被使用，请选择其他序号。\n", newId);
            return;
        }

        // 更新书籍信息
        bookToUpdate->id = newId;
        strcpy(bookToUpdate->name, newName);
        strcpy(bookToUpdate->category, newCategory);
        strcpy(bookToUpdate->status, newStatus);
        bookToUpdate->readTime = newReadTime;

        printf("书籍信息已更新：序号：%d; 书名：%s; 分类：%s; 状态：%s; 阅读耗时：%.1f天\n", bookToUpdate->id, bookToUpdate->name, bookToUpdate->category, bookToUpdate->status, bookToUpdate->readTime);
        saveBooks(list); // 自动保存书籍信息
    } else {
        printf("未找到序号为 %d 或书名为 %s 的书籍。\n", id, name);
    }

    printf("按任意键返回。。。\n");
    getch();
}

// 用户借阅书籍功能函数
void borrowBook(BookList* list) {
    int id = 0;
    char name[NUMBER];
    printf("请输入要借阅的书籍序号或名称：\n");
    printf("序号：");
    scanf("%d", &id);
    getchar(); // 消耗换行符
    printf("书名：");
    fgets(name, NUMBER, stdin);
    // 移除末尾的换行符
    if (name[strlen(name) - 1] == '\n') {
        name[strlen(name) - 1] = '\0';
    }
    Book* book = NULL;
    // 优先根据序号查找
    if (id > 0) {
        book = findBookById(list, id);
    }
    // 如果序号未找到或无效，且书名不为空，使用书名查找
    if (book == NULL && strlen(name) > 0) {
        book = findBookByName(list, name);
    }
    // 如果找到了书籍
    if (book != NULL) {
        // 检查书籍状态
        if (strcmp(book->status, "在馆") == 0) {
            // 更新书籍状态为在借
            strcpy(book->status, "在借");
            printf("您已成功借阅书籍：%s; 序号：%d\n", book->name, book->id);
            saveBooks(list); // 自动保存书籍信息
        } else if (strcmp(book->status, "在借") == 0) {
            printf("这本书正在借出，馆内暂时没有。\n");
        }
    } else {
        printf("这本书馆内没有。\n");
    }
}

// 生成阅读计划
void generateReadingPlan(BookList* list) {
    // 定义所有分类
    const char* categories[] = {"教科书", "历史", "经济", "文学", "哲学", "地理", "数学", "技术", "医学", "艺术", "宗教", "农学", "管理", "儿童"};
    int nCategories = sizeof(categories) / sizeof(categories[0]);
    float totalReadTime = 0;
    // 初始化哈希表
    CategoryBooks categoryBooksHash[nCategories];
    generateCategoryBooksHash(list, categoryBooksHash, categories, nCategories);
    // 存储已选择的书籍以避免重复选择
    int selectedBooks[100] = {0}; // 假设有100本书
    // 遍历每个分类
    for (int i = 0; i < nCategories; i++) {
        Book* book = findAndRemoveBookByCategory(categoryBooksHash, categories[i], selectedBooks, categories, nCategories);
        if (book == NULL) {
            // 当前分类没有可用书籍，寻找其他分类中阅读时间最长的书籍作为替代
            for (int j = 0; j < nCategories; j++) {
                if (j == i) continue;
                book = findAndRemoveBookByCategory(categoryBooksHash, categories[j], selectedBooks, categories, nCategories);
                if (book != NULL) {
                    printf("替代类别: %s, 序号: %d, 书名: %s, 阅读耗时: %.1f 天\n", categories[i], book->id, book->name, book->readTime);
                    break;
                }
            }
        }
        if (book != NULL) {
            // 打印并计算当前分类的阅读时间
            printf("分类: %s, 序号: %d, 书名: %s, 阅读耗时: %.1f 天\n", categories[i], book->id, book->name, book->readTime);
            totalReadTime += book->readTime;
        }
    }

    // 输出总阅读时间
    printf("总的阅读耗时: %.1f 天\n", totalReadTime);
    printf("按任意键返回。。。\n");
    getch();
}
//**********************************************************************************************************************************************************//

//**************************************账号和书籍信息永久保存在电脑功能函数***********************************************************************************//
// 保存书籍信息到books.txt
void saveBooks(BookList* list) {
    FILE* file = fopen(BOOKS_FILE, "w");
    if (file == NULL) {
        printf("无法打开文件 %s 进行写入。\n", BOOKS_FILE);
        return;
    }

    Book* curr = list->head;
    while (curr != NULL) {
        fprintf(file, "%d,%s,%s,%s,%.1f\n",curr->id, curr->name, curr->category,curr->status, curr->readTime);curr = curr->next;
    }

    fclose(file);
    printf("书籍已保存到文件 %s。\n", BOOKS_FILE);
}
// 从books.txt加载书籍信息到程序
void loadBooks(BookList* list) {
    FILE* file = fopen(BOOKS_FILE, "r");
    if (file == NULL) {
        printf("无法打开文件 %s 进行读取。\n", BOOKS_FILE);
        return;
    }

    initList(list);
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        Book* newBook = (Book*)malloc(sizeof(Book));
        sscanf(line, "%d,%49[^,],%19[^,],%9[^,],%f",
               &newBook->id, newBook->name, newBook->category,
               newBook->status, &newBook->readTime);

        newBook->next = list->head;
        newBook->prev = NULL;

        if (list->head != NULL) {
            list->head->prev = newBook;
        }

        list->head = newBook;
        list->count++;
    }

    fclose(file);
    printf("书籍已从文件 %s 加载。\n", BOOKS_FILE);
}

// 保存账号信息到文件
void saveAccounts(){
    FILE* file = fopen(ACCOUNTS_FILE, "w");
    if (file == NULL) {
        printf("无法打开账号文件进行保存。\n");
        return;
    }
    for (int i = 0; i < numAccounts; i++) {
        fprintf(file, "%s %s\n", accounts[i].username, accounts[i].password);
    }
    printf("账号已保存到文件 %s 。\n", ACCOUNTS_FILE);
    fclose(file);
}


// 从文件加载账号信息
void loadAccounts() {
    FILE* file = fopen(ACCOUNTS_FILE, "r");
    if (file == NULL) {
        printf("无法打开账号文件进行加载。\n");
        return;
    }
    numAccounts = 0;
    while (fscanf(file, "%s %s", accounts[numAccounts].username, accounts[numAccounts].password) != EOF) {
        numAccounts++;
    }
    printf("账号已从文件 %s 加载。\n", ACCOUNTS_FILE);
    fclose(file);
}
//*****************************************************************************************************************************************************//

//*************************************人机交互界面功能函数***************************************************************************************************//
// 检查账号用户名是否已存在的函数
bool usernameExists(const char* username) {
    for (int i = 0; i < numAccounts; i++) {
        if (strcmp(accounts[i].username, username) == 0) {
            return true;
        }
    }
    return false;
}

// 创建账号的函数
void creatAccount(const char* username, const char* password) {
    if (usernameExists(username)) {
        printf("用户名已存在。请使用不同的用户名。\n");
        printf("返回主菜单\n");
        return;
    }
    if (numAccounts < MAX_ACCOUNTS) {
        strcpy(accounts[numAccounts].username, username);
        strcpy(accounts[numAccounts].password, password);
        numAccounts++;
        printf("账号创建成功。\n");
        saveAccounts(); // 自动保存账号
        printf("返回主菜单\n");
    } else {
        printf("无法添加更多账号。已达到最大限制。\n");
        printf("返回主菜单\n");
    }
}

// 创建账号的人机交互函数
void addAccount(){
    printf("\n");
    printf("\t\t     ======================    \n");
    printf("\t\t             创建账号      \n");
    printf("\t\t     ======================    \n\n");
    char username[NUMBER];
    char password[NUMBER];

    printf("请输入想要创建的用户名：");
    scanf("%s", username);
    printf("请输入想要创建的密码：");
    scanf("%s", password);

    creatAccount(username, password);
}


// 验证账号是否正确的函数
bool verifyCredentials(const char* username, const char* password) {
    for (int i = 0; i < numAccounts; i++) {
        if (strcmp(accounts[i].username, username) == 0 && strcmp(accounts[i].password, password) == 0) {
            return true; // 验证成功
        }
    }
    return false; // 验证失败
}

// 显示所有账号，第一个为管理员账号
void displayAccounts() {
    if (numAccounts == 0) {
        printf("没有账号。\n");
        return;
    }

    for (int i = 0; i < numAccounts; i++) {
        if (i == 0) {
            printf("账号 %d: %s (管理员)\n", i + 1, accounts[i].username);
        } else {
            printf("账号 %d: %s\n", i + 1, accounts[i].username);
        }
    }
    printf("按任意键返回。。。\n");
    getch();
}


// 删除指定账号（除了第一个管理员账号）
void deleteAccount(const char* username) {
    if (strcmp(username, accounts[0].username) == 0) {
        printf("不能删除管理员账号。\n");
        return;
    }

    int index = -1;
    for (int i = 1; i < numAccounts; i++) {
        if (strcmp(accounts[i].username, username) == 0) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        printf("未找到账号 %s。\n", username);
        return;
    }

    for (int i = index; i < numAccounts - 1; i++) {
        accounts[i] = accounts[i + 1];
    }
    numAccounts--;
    saveAccounts();
    printf("账号 %s 已删除。\n", username);
    printf("按任意键返回。。。\n");
    getch();
}

// 删除所有账号（除了第一个管理员账号）
void deleteAllAccounts() {
    if (numAccounts <= 1) {
        printf("没有可删除的账号。\n");
        return;
    }

    numAccounts = 1; // 只保留第一个管理员账号
    saveAccounts();
    printf("所有账号已删除（除了管理员账号）。\n");
    printf("按任意键返回。。。\n");
    getch();
}


// 修改指定账号密码的函数
void changePassword() {
    printf("\n");
    printf("\t\t     ======================    \n");
    printf("\t\t         欢迎进入修改密码      \n");
    printf("\t\t     ======================    \n");
    printf("\n");

    char username[NUMBER];
    char password[NUMBER];
    char new_password1[NUMBER];   //存放第一次新密码
    char new_password2[NUMBER];  //存放第二次新密码

    printf("请输入原账号用户名：");
    scanf("%s", username);
    printf("请输入原账号密码：");
    scanf("%s", password);

    if (!verifyCredentials(username, password)) {
        printf("用户名或密码错误。拒绝访问。\n");
        return;
    }else{
        printf("账号密码正确，进入修改密码\n");
    }

    printf("请输入新密码：\n");
    scanf("%s",&new_password1);
    printf("请再输一次：\n");
    scanf("%s",&new_password2);
    if(!strcmp(new_password1,new_password2)) //比较 两次修改后输入的密码
    {
        for (int i = 0; i < numAccounts; i++) {
            if (strcmp(accounts[i].username, username) == 0) {
                strcpy(accounts[i].password, new_password2);
                printf("密码修改成功;账号：%s; 新密码：%s; 请牢记！！\n", username,new_password2);
                saveAccounts(); // 自动保存账号
                printf("按任意键返回。。。\n");
                getch();
                return;
            }
        }
    }
    else
    {
        printf("错误！两次输入的密码不一致，按任意键返回。。。\n");
        getch();
        return;
    }
}
// 管理员模式
void Administrator(BookList* list) {
    printf("\n");
    printf("\t\t     ======================    \n");
    printf("\t\t         欢迎使用管理员模式      \n");
    printf("\t\t     ======================    \n");
    printf("请登录：\n");

    char username[NUMBER];
    char password[NUMBER];

    printf("请输入管理员用户名：");
    scanf("%s", username);
    printf("请输入管理员密码：");
    scanf("%s", password);

    if ((!strcmp(password,accounts[0].password)) && (!strcmp(username,accounts[0].username))) {
        printf("账号密码正确，进入管理员菜单\n");
    }else{
        printf("用户名或密码错误。拒绝访问。\n");
        return;
    }

    int choice;
    do {
        printf("\n");
        printf("*****************************************\n");
        printf("*           ---- 管理员菜单 ----          *\n");
        printf("*****************************************\n");
        printf("****************书籍修改操作***************\n");
        printf("* 1. 添加书籍                             *\n");
        printf("* 2. 删除书籍（按序号或书名）                *\n");
        printf("* 3. 更新书籍状态（按序号或书名）             *\n");
        printf("* 4. 更新书籍所有信息（按序号或书名）          *\n");
        printf("****************书籍显示操作***************\n");
        printf("* 5. 按序号查找单个书籍                     *\n");
        printf("* 6. 按书名查找单个书籍                     *\n");
        printf("* 7. 按分类显示所有书籍                     *\n");
        printf("* 8. 按在馆状态显示所有书籍                  *\n");
        printf("* 9. 按序号显示所有书籍                     *\n");
        printf("* 10. 删除所有书籍                         *\n");
        printf("****************账号修改操作****************\n");
        printf("* 11. 显示所有账号                         *\n");
        printf("* 12. 删除指定账号（管理员账号无法删除）       *\n");
        printf("* 13. 删除所有账号（管理员账号无法删除）       *\n");
        printf("* 14. 修改管理员账号密码                    *\n");
        printf("* 15. 生成阅读计划                         *\n");
        printf("* 16. 返回主菜单                           *\n");
        printf("******************************************\n");
        printf("请选择您的操作: ");
        scanf("%d", &choice);

        int id;
        char name[NUMBER];
        char account[NUMBER];
        char category[NUMBER];
        char status[NUMBER];
        Book* book = NULL;
        float readTime;

        switch (choice) {
            case 1:
                printf("请输入书籍信息（序号 书名 分类 状态 阅读耗时）: ");
                scanf("%d %s %s %s %f", &id, name, category, status, &readTime);
                addBook(list, id, name, category, status, readTime);
                quickSort(list);
                saveBooks(list);
                printf("按任意键返回。。。\n");
                getch();
                break;
            case 2:
                printf("请输入书籍序号（不知道则填0）或书籍名称（不知道则填NULL）（空格隔开）: ");
                scanf("%d %s", &id, name);
                deleteBook(list, id, name);
                break;
            case 3:
                printf("请输入书籍序号（不知道则填0）或书籍名称（不知道则填NULL）（空格隔开）: ");
                scanf("%d %s", &id, name);
                printf("请输入书籍新状态: ");
                scanf("%s", status);
                updateBookStatus(list, id, name, status);
                break;
            case 4:
                updateBook(list);
                break;
            case 5:
                printf("请输入要查找的书籍序号: ");
                scanf("%d", &id);
                book = findBookById(list, id);
                if (book != NULL) {
                    printf("找到指定序号的书籍：%s; 序号：%d; 分类：%s; 状态：%s; 阅读耗时：%.1f天\n", book->name, book->id, book->category, book->status, book->readTime);
                } else {
                    printf("\n未找到指定序号的书籍。\n");
                }
                printf("按任意键返回。。。\n");
                getch();
                break;
            case 6:
                printf("请输入要查找的书籍名称: ");
                scanf("%s", name);
                book = findBookByName(list, name);
                if (book != NULL) {
                    printf("找到指定书名的书籍：%s; 序号：%d; 分类：%s; 状态：%s; 阅读耗时：%.1f天\n", book->name, book->id, book->category, book->status, book->readTime);
                } else {
                    printf("\n未找到指定书名的书籍。\n");
                }
                printf("按任意键返回。。。\n");
                getch();
                break;
            case 7:
                showBooksByAllCategories(list);
                break;
            case 8:
                showBooksByStatus(list);
                break;
            case 9:
                printBooks(list);
                break;
            case 10:
                deleteAllBooks(list);
                break;
            case 11:
                displayAccounts();
                break;
            case 12:
                printf("请输入要删除的账号用户名: ");
                scanf("%s", account);
                deleteAccount(account);
                break;
            case 13:
                deleteAllAccounts();
                break;
            case 14:
                changePassword();
                break;
            case 15:
                generateReadingPlan(list);
                break;
            case 16:
                printf("返回主菜单\n");
                break;
            default:
                printf("无效的选项，请重新选择。\n");
        }
    } while (choice != 16);
}
// 普通用户模式
void UserInterface(BookList* list) {
    printf("\n");
    printf("\t\t     ======================    \n");
    printf("\t\t        欢迎使用用户模式        \n");
    printf("\t\t     ======================    \n");
    printf("请登录：\n");

    char username[NUMBER];
    char password[NUMBER];

    printf("请输入用户名：");
    scanf("%s", username);
    printf("请输入密码：");
    scanf("%s", password);

    if (!verifyCredentials(username, password)) {
        printf("用户名或密码错误。拒绝访问。\n");
        return;
    }

    printf("账号密码正确，进入用户菜单\n");

    int choice;
    do {
        printf("\n");
        printf("*****************************************\n");
        printf("*           ---- 用户菜单 ----            *\n");
        printf("*****************************************\n");
        printf("* 1. 按序号查找单个书籍                     *\n");
        printf("* 2. 按书名查找单个书籍                     *\n");
        printf("* 3. 按分类显示所有书籍                     *\n");
        printf("* 4. 按在馆状态显示所有书籍                  *\n");
        printf("* 5. 按序号显示所有书籍                     *\n");
        printf("* 6. 借阅书籍                             *\n");
        printf("* 7. 生成阅读计划                          *\n");
        printf("* 8. 修改账号密码                          *\n");
        printf("* 9. 返回主菜单                           *\n");
        printf("*****************************************\n");
        printf("请选择您的操作: ");
        scanf("%d", &choice);

        int id;
        char name[NUMBER];
        Book* book = NULL;

        switch (choice) {
            case 1:
                printf("请输入要查找的书籍序号: ");
                scanf("%d", &id);
                book = findBookById(list, id);
                if (book != NULL) {
                    printf("找到指定序号的书籍：%s; 序号：%d; 分类：%s; 状态：%s; 阅读耗时：%.1f天\n", book->name, book->id, book->category, book->status, book->readTime);
                } else {
                    printf("\n未找到指定序号的书籍。\n");
                }
                printf("按任意键返回。。。\n");
                getch();
                break;
            case 2:
                printf("请输入要查找的书籍名称: ");
                scanf("%s", name);
                book = findBookByName(list, name);
                if (book != NULL) {
                    printf("找到指定书名的书籍：%s; 序号：%d; 分类：%s; 状态：%s; 阅读耗时：%.1f天\n", book->name, book->id, book->category, book->status, book->readTime);
                } else {
                    printf("\n未找到指定书名的书籍。\n");
                }
                printf("按任意键返回。。。\n");
                getch();
                break;
            case 3:
                showBooksByAllCategories(list);
                break;
            case 4:
                showBooksByStatus(list);
                break;
            case 5:
                printBooks(list);
                break;
            case 6:
                borrowBook(list);
                break;
            case 7:
                generateReadingPlan(list);
                break;
            case 8:
                changePassword();
                break;
            case 9:
                printf("返回主菜单\n");
                break;
            default:
                printf("无效的选项，请重新选择。\n");
        }
    } while (choice != 9);
}

// 主菜单
void main_menu() {
    BookList list;
    initList(&list);

    loadAccounts(); // 从文本文件加载账号信息
    loadBooks(&list); // 从文本文件加载书籍信息

    // 添加初始书籍信息
//    addBook(&list, 1, "大学物理", "教科书", "在馆", 30);
//    addBook(&list, 11, "史记", "历史", "在馆", 15);
//    addBook(&list, 12, "资治通鉴", "历史", "在借", 27);
//    addBook(&list, 21, "资本论", "经济", "在馆", 25);
//    addBook(&list, 31, "哈姆雷特", "文学", "在馆", 1);
//    addBook(&list, 36, "存在与时间", "哲学", "在馆", 6);
//    addBook(&list, 41, "地理信息系统导论", "地理", "在馆", 3);
//    addBook(&list, 46, "几何原本", "数学", "在馆", 3);
//    addBook(&list, 51, "计算机网络：自顶向下方法", "技术", "在借", 18);
//    addBook(&list, 56, "格氏解剖学", "医学", "在馆", 43);
//    addBook(&list, 61, "艺术的故事", "艺术", "在馆", 7);
//    addBook(&list, 66, "宗教的解释", "宗教", "在馆", 5);
//    addBook(&list, 71, "植物育种原理", "农学", "在馆", 2);
//    addBook(&list, 76, "创新者的窘境", "管理", "在馆", 0.5);
//    addBook(&list, 81, "小王子", "儿童", "在借", 0.5);

    // 添加额外书籍信息，每一类书籍均添加三本书
//    addBook(&list, 2, "物理学原理", "教科书", "在馆", 25);
//    addBook(&list, 9, "控制理论基础", "教科书", "在馆", 24);
//    addBook(&list, 10, "电路分析基础", "教科书", "在馆", 27);
//    addBook(&list, 13, "中国通史", "历史", "在借", 23);
//    addBook(&list, 14, "世界史纲要", "历史", "在借", 19);
//    addBook(&list, 16, "古代文明史", "历史", "在借", 17);
//    addBook(&list, 28, "经济体制改革", "经济", "在馆", 24);
//    addBook(&list, 29, "经济数学基础", "经济", "在馆", 27);
//    addBook(&list, 30, "经济法学", "经济", "在馆", 29);
//    addBook(&list, 33, "红楼梦", "文学", "在馆", 10);
//    addBook(&list, 34, "人间失格", "文学", "在馆", 7);
//    addBook(&list, 35, "百年孤独", "文学", "在馆", 3);
//    addBook(&list, 37, "西方哲学史", "哲学", "在馆", 8);
//    addBook(&list, 39, "逻辑学基础", "哲学", "在馆", 12);
//    addBook(&list, 40, "心灵哲学", "哲学", "在馆", 15);
//    addBook(&list, 43, "地球科学导论", "地理", "在馆", 8);
//    addBook(&list, 44, "自然灾害学", "地理", "在馆", 10);
//    addBook(&list, 45, "地理学原理", "地理", "在馆", 5);
//    addBook(&list, 48, "数论导论", "数学", "在馆", 4);
//    addBook(&list, 49, "拓扑学基础", "数学", "在馆", 7);
//    addBook(&list, 50, "概率论导论", "数学", "在馆", 6);
//    addBook(&list, 53, "软件工程导论", "技术", "在借", 20);
//    addBook(&list, 54, "数据库系统原理", "技术", "在借", 22);
//    addBook(&list, 55, "编程语言原理", "技术", "在借", 16);
//    addBook(&list, 57, "内科学教程", "医学", "在馆", 40);
//    addBook(&list, 58, "外科学导论", "医学", "在馆", 45);
//    addBook(&list, 59, "药理学基础", "医学", "在馆", 38);
//    addBook(&list, 63, "绘画技法与艺术欣赏", "艺术", "在馆", 5);
//    addBook(&list, 64, "建筑设计与欣赏", "艺术", "在馆", 9);
//    addBook(&list, 65, "音乐鉴赏与历史", "艺术", "在馆", 8);
//    addBook(&list, 68, "伊斯兰教史", "宗教", "在馆", 4);
//    addBook(&list, 69, "基督教信仰与历史", "宗教", "在馆", 6);
//    addBook(&list, 70, "印度教文化与宗教", "宗教", "在馆", 3);
//    addBook(&list, 73, "作物遗传育种", "农学", "在馆", 4);
//    addBook(&list, 74, "园艺植物学", "农学", "在馆", 3);
//    addBook(&list, 75, "农村区域发展规划", "农学", "在馆", 6);
//    addBook(&list, 78, "团队建设与管理", "管理", "在馆", 0.7);
//    addBook(&list, 79, "市场营销策略", "管理", "在馆", 1.2);
//    addBook(&list, 80, "项目管理实务", "管理", "在馆", 0.8);
//    addBook(&list, 83, "格林童话", "儿童", "在借", 0.5);
//    addBook(&list, 84, "哈利波特与魔法石", "儿童", "在借", 1.5);
//    addBook(&list, 85, "鲁宾逊漂流记", "儿童", "在借", 1);



    // 对书籍按序号进行排序
    quickSort(&list);
    saveBooks(&list);

    printf("\n\n");
    printf("*****************************************\n");
    printf("*    欢迎进入刘嘉俊的图书借阅计划管理系统      *\n");
    printf("*****************************************\n");
    printf("\n");

    int choice;
    do {
        menu:
        printf("\n");
        printf("*****************************************\n");
        printf("*           ---- 主菜单 ----             *\n");
        printf("*****************************************\n");
        printf("* 1. 管理员登录                           *\n");
        printf("* 2. 普通用户登录                          *\n");
        printf("* 3. 修改指定账号的密码                     *\n");
        printf("* 4. 创建账号                             *\n");
        printf("* 5. 退出                                *\n");
        printf("*****************************************\n");
        printf("请选择您的操作: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                Administrator(&list);
                break;
            case 2:
                UserInterface(&list);
                break;
            case 3:
                changePassword();
                break;
            case 4:
                addAccount();
                goto menu;
            case 5:
                printf("退出系统\n");
                break;
            default:
                printf("无效的选项，请重新选择。\n");
        }
    } while (choice != 5);
}
//*****************************************************************************************************************************************************//

int main() {

    main_menu();

    return 0;
}

