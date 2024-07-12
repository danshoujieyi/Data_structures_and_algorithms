/*

 *
 */
//****************************ͷ�ļ�******************************************************************************//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
//*******************************************************************************************************************//

//*********************************�����ṹ����궨��******************************************************************************//
#define MAX_ACCOUNTS 10 // ����˺�����
#define NUMBER       30 // �����滻��
#define ACCOUNTS_FILE "C:\\Users\\liujiajun\\Desktop\\shujujiegoukeshe\\CLion_C23\\accounts.txt" // ����·�� �˺��ļ�
#define BOOKS_FILE "C:\\Users\\liujiajun\\Desktop\\shujujiegoukeshe\\CLion_C23\\books.txt" // ����·�� �鼮�ļ�

typedef struct {
    char username[NUMBER];
    char password[NUMBER];
} Account;

// �洢����˺ŵ�����
Account accounts[MAX_ACCOUNTS];
int numAccounts = 0; // ��0�����óɹ���Ա�˺ţ�0λ�������ٴ����˺�
//ԭʼ����Ա�˺��û��� 11
//ԭʼ����Ա�˺�����   22

// ����ͼ��ṹ�壬�����鼮�Ļ�����Ϣ
typedef struct Book {
    int id;              // ͼ�����
    char name[NUMBER];       // ͼ������
    char category[NUMBER];   // ��������
    char status[NUMBER];     // ����״̬
    float readTime;      // �Ķ���ʱ
    struct Book* next;   // ָ����һ�����ָ��
    struct Book* prev;
} Book;

// ����ͼ������ṹ�壬���������ͷָ����鼮����
typedef struct {
    int count;
    Book* head;
} BookList;

// ��������鼮��ϣ��
typedef struct {
    Book* books[100]; // ����ÿ���������100����
    int size;
} CategoryBooks;

void saveBooks(BookList* list); // �����鼮��Ϣ���ı��ļ�books.txt����ͷ
//***********************************************************************************************************************************//

//*********************************�㷨���ܺ���**********************************************************************************************//
// ���������㷨
// ���������鼮�ڵ������
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
// �������������ڿ�������
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
// ��������ĵݹ�ʵ�ֺ���
void quickSortUtil(Book* low, Book* high) {
    if (high != NULL && low != high && low != high->next) {
        Book* pivot = partition(low, high);
        quickSortUtil(low, pivot->prev);
        quickSortUtil(pivot->next, high);
    }
}
// ���������㷨
void quickSort(BookList* list) {
    if (list->count <= 1) return;
    Book* last = list->head;
    while (last->next != NULL) {
        last = last->next;
    }
    quickSortUtil(list->head, last);
    printf("�鼮ʹ�ÿ��������㷨�Ѱ��������\n");
}

// ���ɷ����鼮��ϣ��
void generateCategoryBooksHash(BookList* list, CategoryBooks* categoryBooksHash, const char* categories[], int nCategories) {
    for (int i = 0; i < nCategories; i++) {
        categoryBooksHash[i].size = 0;
    }

    Book* current = list->head;
    while (current != NULL) {
        for (int j = 0; j < nCategories; j++) {
            if (strcmp(current->category, categories[j]) == 0 && strcmp(current->status, "�ڹ�") == 0) {
                categoryBooksHash[j].books[categoryBooksHash[j].size++] = current;
                break;
            }
        }
        current = current->next;
    }

    // ��ÿ��������鼮���Ķ�ʱ������
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

// �ӹ�ϣ���в��Ҳ��Ƴ��ض�������鼮
Book* findAndRemoveBookByCategory(CategoryBooks* categoryBooksHash, const char* category, int* selectedBooks, const char* categories[], int nCategories) {
    for (int i = 0; i < nCategories; i++) {
        if (strcmp(categories[i], category) == 0) {
            for (int j = 0; j < categoryBooksHash[i].size; j++) {
                Book* book = categoryBooksHash[i].books[j];
                if (selectedBooks[book->id - 1] == 0) {
                    selectedBooks[book->id - 1] = 1;
                    // �Ƴ��鼮
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

//**************************************�鼮��ز������ܺ���*******************************************************************************/
// ��ʼ��ͼ��������
void initList(BookList* list) {
    list->count = 0;
    list->head = NULL;
}

// ����鼮��������
void addBook(BookList* list, int id, const char* name, const char* category, const char* status, float readTime) {
    Book* newBook = (Book*)malloc(sizeof(Book));
    if (newBook == NULL) {
        printf("�ڴ����ʧ��\n");
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
        list->head->prev = newBook;  // �������Ϊ�գ�����ԭͷ�ڵ�� prev ָ��
    }

    list->head = newBook;
    list->count++;
    printf("���ѳɹ�����鼮��%s; ��ţ�%d; ���ࣺ%s; ״̬��%s; �Ķ���ʱ��%.1f��\n", newBook->name, id, newBook->category, newBook->status,newBook->readTime);
    saveBooks(list); // �Զ������鼮��Ϣ
}

// ����ָ����ŵ��鼮
Book* findBookById(BookList* list, int id) {
    Book* curr = list->head;
    // �ҵ�ָ����ŵ��鼮�ڵ�
    while (curr != NULL && curr->id != id) {
        curr = curr->next;
    }
    return curr;  // �����ҵ����鼮�ڵ㣬���δ�ҵ��򷵻�NULL
}

// ����ָ���������鼮
Book* findBookByName(BookList* list, const char* name) {
    Book* curr = list->head;
    // �ҵ�ָ���������鼮�ڵ�
    while (curr != NULL && strcmp(curr->name, name) != 0) {
        curr = curr->next;
    }
    return curr;  // �����ҵ����鼮�ڵ㣬���δ�ҵ��򷵻�NULL
}

// ��������ʾ�����鼮��Ϣ
void showBooksByAllCategories(BookList* list) {
    const char* categories[] = {"�̿���", "��ʷ", "����", "��ѧ", "��ѧ", "����", "��ѧ", "����", "ҽѧ", "����", "�ڽ�", "ũѧ", "����", "��ͯ"};
    int numCategories = sizeof(categories) / sizeof(categories[0]);

    for (int i = 0; i < numCategories; i++) {
        const char* category = categories[i];
        Book* current = list->head;
        bool found = false;
        printf("����Ϊ %s ���鼮�б�:\n", category);
        while (current != NULL) {
            if (strcmp(current->category, category) == 0) {
                printf("��ţ�%d, ������%s, ���ࣺ%s, ״̬��%s, �Ķ���ʱ��%.1f��\n", current->id, current->name, current->category, current->status, current->readTime);
                found = true;
            }
            current = current->next;
        }
        if (!found) {
            printf("δ�ҵ�����Ϊ %s ���鼮��\n", category);
        }
        printf("\n"); // ÿ������֮���һ��
    }
    printf("����������ء�����\n");
    getch();
}

// ���ڹ�״̬��ʾ�����鼮��Ϣ
void showBooksByStatus(BookList* list) {
    const char* statuses[] = {"�ڹ�", "�ڽ�"};
    int numStatuses = sizeof(statuses) / sizeof(statuses[0]);

    for (int i = 0; i < numStatuses; i++) {
        const char* status = statuses[i];
        Book* current = list->head;
        bool found = false;
        printf("״̬Ϊ %s ���鼮�б�:\n", status);
        while (current != NULL) {
            if (strcmp(current->status, status) == 0) {
                printf("��ţ�%d, ������%s, ���ࣺ%s, ״̬��%s, �Ķ���ʱ��%.1f��\n", current->id, current->name, current->category, current->status, current->readTime);
                found = true;
            }
            current = current->next;
        }
        if (!found) {
            printf("δ�ҵ�״̬Ϊ %s ���鼮��\n", status);
        }
        printf("\n"); // ÿ��״̬֮���һ��
    }
    printf("����������ء�����\n");
    getch();
}

// ����Ŵ�ӡ�����鼮��Ϣ
void printBooks(BookList* list) {
    if (list->count == 0) {
        printf("��ǰû���κ��鼮��\n");
        return;
    }
    Book* curr = list->head;
    while (curr != NULL) {
        printf("���: %d, ����: %s, ����: %s, ״̬: %s, �Ķ���ʱ: %.1f ��\n",
               curr->id, curr->name, curr->category, curr->status, curr->readTime);
        curr = curr->next;
    }
    printf("����������ء�����\n");
    getch();
}

// ɾ���鼮�����Ը����鼮��Ż��������粻֪��������롱0������֪���������롱NULL��
void deleteBook(BookList* list, int id, const char* name) {
    Book* prev = NULL;
    Book* curr = list->head;
    Book* book = NULL;
    // ���ȸ�����Ų���
    if (id > 0) {
        book = findBookById(list, id);
    }
    // ������δ�ҵ�����Ч��ʹ����������
    if (book == NULL && name != NULL) {
        book = findBookByName(list, name);
    }
    // ɾ���鼮
    if (book != NULL) {
        curr = list->head;
        while (curr != NULL && curr != book) {
            prev = curr;
            curr = curr->next;
        }

        if (prev == NULL) {
            list->head = curr->next;  // �����ͷ�ڵ㣬�����ͷָ��
        } else {
            prev->next = curr->next;  // �������ǰһ���ڵ��nextָ��
        }

        list->count--;
        printf("���ѳɹ�ɾ���鼮��%s; ��ţ�%d; ϵͳ���鼮������%d \n", curr->name, curr->id, list->count);
        saveBooks(list); // �Զ������鼮��Ϣ
        free(curr);  // �ͷ��ڴ�
        printf("����������ء�����\n");
        getch();
    } else {
        if (id > 0) {
            printf("û���ҵ����Ϊ %d ���鼮��\n", id);
        } else if (name != NULL) {
            printf("û���ҵ�����Ϊ ��%s�� ���鼮��\n", name);
        }
        printf("����������ء�����\n");
        getch();
    }
}

// ɾ�������鼮
void deleteAllBooks(BookList* list) {
    Book* curr = list->head;
    while (curr != NULL) {
        Book* next = curr->next;
        free(curr);
        curr = next;
    }
    list->head = NULL;
    list->count = 0;
    printf("�����鼮��ɾ����ϵͳ���鼮������%d\n", list->count);
    saveBooks(list); // �Զ������鼮��Ϣ
}

// �����鼮�Ľ���״̬�����Ը����鼮��Ż��������粻֪��������롱0������֪���������롱NULL��
void updateBookStatus(BookList* list, int id, const char* name, const char* newStatus) {
    Book* book = NULL;
    // ���ȸ�����Ų���
    if (id > 0) {
        book = findBookById(list, id);
    }
    // ������δ�ҵ�����Ч��ʹ����������
    if (book == NULL && name != NULL) {
        book = findBookByName(list, name);
    }
    // �����鼮״̬
    if (book != NULL) {
        char oldStatus[NUMBER];
        strcpy(oldStatus, book->status);  // ����ԭ״̬
        strcpy(book->status, newStatus);  // ����Ϊ��״̬
        printf("���ѳɹ������鼮״̬��%s; ��ţ�%d; ԭ״̬��%s; ��״̬Ϊ��%s\n", book->name, book->id, oldStatus, newStatus);
        saveBooks(list); // �Զ������鼮��Ϣ
        printf("����������ء�����\n");
        getch();
    } else {
        if (id > 0) {
            printf("û���ҵ����Ϊ %d ���鼮��\n", id);
        } else if (name != NULL) {
            printf("û���ҵ�����Ϊ ��%s�� ���鼮��\n", name);
        }
        printf("����������ء�����\n");
        getch();
    }
}

// ʹ����Ż����������鼮��Ϣ�������鼮��š����ơ����ࡢ�ڹ�״̬���Ķ���ʱ
void updateBook(BookList* list) {
    int id;
    char name[NUMBER];
    char newName[NUMBER];
    char newCategory[NUMBER];
    char newStatus[NUMBER];
    float newReadTime;
    int newId;

    printf("������Ҫ���µ��鼮����ţ���֪���������0����");
    scanf("%d", &id);
    getchar();  // ������뻺�����еĻ��з�

    if (id == 0) {
        printf("������Ҫ���µ��鼮��������");
        fgets(name, sizeof(name), stdin);
        name[strcspn(name, "\n")] = '\0';  // �Ƴ����з�
    } else {
        name[0] = '\0';  // �������
    }

    printf("�������µ���ţ�");
    scanf("%d", &newId);
    getchar();  // ������뻺�����еĻ��з�

    printf("�������µ�������");
    fgets(newName, sizeof(newName), stdin);
    newName[strcspn(newName, "\n")] = '\0'; // �Ƴ����з�
    printf("�������µķ��ࣺ");
    fgets(newCategory, sizeof(newCategory), stdin);
    newCategory[strcspn(newCategory, "\n")] = '\0'; // �Ƴ����з�
    printf("�������µ�״̬��");
    fgets(newStatus, sizeof(newStatus), stdin);
    newStatus[strcspn(newStatus, "\n")] = '\0';
    printf("�������µ��Ķ���ʱ���죩��");
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
        // ���������Ƿ�ʹ��
        if (newId != bookToUpdate->id && findBookById(list, newId) != NULL) {
            printf("����� %d �ѱ�ʹ�ã���ѡ��������š�\n", newId);
            return;
        }

        // �����鼮��Ϣ
        bookToUpdate->id = newId;
        strcpy(bookToUpdate->name, newName);
        strcpy(bookToUpdate->category, newCategory);
        strcpy(bookToUpdate->status, newStatus);
        bookToUpdate->readTime = newReadTime;

        printf("�鼮��Ϣ�Ѹ��£���ţ�%d; ������%s; ���ࣺ%s; ״̬��%s; �Ķ���ʱ��%.1f��\n", bookToUpdate->id, bookToUpdate->name, bookToUpdate->category, bookToUpdate->status, bookToUpdate->readTime);
        saveBooks(list); // �Զ������鼮��Ϣ
    } else {
        printf("δ�ҵ����Ϊ %d ������Ϊ %s ���鼮��\n", id, name);
    }

    printf("����������ء�����\n");
    getch();
}

// �û������鼮���ܺ���
void borrowBook(BookList* list) {
    int id = 0;
    char name[NUMBER];
    printf("������Ҫ���ĵ��鼮��Ż����ƣ�\n");
    printf("��ţ�");
    scanf("%d", &id);
    getchar(); // ���Ļ��з�
    printf("������");
    fgets(name, NUMBER, stdin);
    // �Ƴ�ĩβ�Ļ��з�
    if (name[strlen(name) - 1] == '\n') {
        name[strlen(name) - 1] = '\0';
    }
    Book* book = NULL;
    // ���ȸ�����Ų���
    if (id > 0) {
        book = findBookById(list, id);
    }
    // ������δ�ҵ�����Ч����������Ϊ�գ�ʹ����������
    if (book == NULL && strlen(name) > 0) {
        book = findBookByName(list, name);
    }
    // ����ҵ����鼮
    if (book != NULL) {
        // ����鼮״̬
        if (strcmp(book->status, "�ڹ�") == 0) {
            // �����鼮״̬Ϊ�ڽ�
            strcpy(book->status, "�ڽ�");
            printf("���ѳɹ������鼮��%s; ��ţ�%d\n", book->name, book->id);
            saveBooks(list); // �Զ������鼮��Ϣ
        } else if (strcmp(book->status, "�ڽ�") == 0) {
            printf("�Ȿ�����ڽ����������ʱû�С�\n");
        }
    } else {
        printf("�Ȿ�����û�С�\n");
    }
}

// �����Ķ��ƻ�
void generateReadingPlan(BookList* list) {
    // �������з���
    const char* categories[] = {"�̿���", "��ʷ", "����", "��ѧ", "��ѧ", "����", "��ѧ", "����", "ҽѧ", "����", "�ڽ�", "ũѧ", "����", "��ͯ"};
    int nCategories = sizeof(categories) / sizeof(categories[0]);
    float totalReadTime = 0;
    // ��ʼ����ϣ��
    CategoryBooks categoryBooksHash[nCategories];
    generateCategoryBooksHash(list, categoryBooksHash, categories, nCategories);
    // �洢��ѡ����鼮�Ա����ظ�ѡ��
    int selectedBooks[100] = {0}; // ������100����
    // ����ÿ������
    for (int i = 0; i < nCategories; i++) {
        Book* book = findAndRemoveBookByCategory(categoryBooksHash, categories[i], selectedBooks, categories, nCategories);
        if (book == NULL) {
            // ��ǰ����û�п����鼮��Ѱ�������������Ķ�ʱ������鼮��Ϊ���
            for (int j = 0; j < nCategories; j++) {
                if (j == i) continue;
                book = findAndRemoveBookByCategory(categoryBooksHash, categories[j], selectedBooks, categories, nCategories);
                if (book != NULL) {
                    printf("������: %s, ���: %d, ����: %s, �Ķ���ʱ: %.1f ��\n", categories[i], book->id, book->name, book->readTime);
                    break;
                }
            }
        }
        if (book != NULL) {
            // ��ӡ�����㵱ǰ������Ķ�ʱ��
            printf("����: %s, ���: %d, ����: %s, �Ķ���ʱ: %.1f ��\n", categories[i], book->id, book->name, book->readTime);
            totalReadTime += book->readTime;
        }
    }

    // ������Ķ�ʱ��
    printf("�ܵ��Ķ���ʱ: %.1f ��\n", totalReadTime);
    printf("����������ء�����\n");
    getch();
}
//**********************************************************************************************************************************************************//

//**************************************�˺ź��鼮��Ϣ���ñ����ڵ��Թ��ܺ���***********************************************************************************//
// �����鼮��Ϣ��books.txt
void saveBooks(BookList* list) {
    FILE* file = fopen(BOOKS_FILE, "w");
    if (file == NULL) {
        printf("�޷����ļ� %s ����д�롣\n", BOOKS_FILE);
        return;
    }

    Book* curr = list->head;
    while (curr != NULL) {
        fprintf(file, "%d,%s,%s,%s,%.1f\n",curr->id, curr->name, curr->category,curr->status, curr->readTime);curr = curr->next;
    }

    fclose(file);
    printf("�鼮�ѱ��浽�ļ� %s��\n", BOOKS_FILE);
}
// ��books.txt�����鼮��Ϣ������
void loadBooks(BookList* list) {
    FILE* file = fopen(BOOKS_FILE, "r");
    if (file == NULL) {
        printf("�޷����ļ� %s ���ж�ȡ��\n", BOOKS_FILE);
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
    printf("�鼮�Ѵ��ļ� %s ���ء�\n", BOOKS_FILE);
}

// �����˺���Ϣ���ļ�
void saveAccounts(){
    FILE* file = fopen(ACCOUNTS_FILE, "w");
    if (file == NULL) {
        printf("�޷����˺��ļ����б��档\n");
        return;
    }
    for (int i = 0; i < numAccounts; i++) {
        fprintf(file, "%s %s\n", accounts[i].username, accounts[i].password);
    }
    printf("�˺��ѱ��浽�ļ� %s ��\n", ACCOUNTS_FILE);
    fclose(file);
}


// ���ļ������˺���Ϣ
void loadAccounts() {
    FILE* file = fopen(ACCOUNTS_FILE, "r");
    if (file == NULL) {
        printf("�޷����˺��ļ����м��ء�\n");
        return;
    }
    numAccounts = 0;
    while (fscanf(file, "%s %s", accounts[numAccounts].username, accounts[numAccounts].password) != EOF) {
        numAccounts++;
    }
    printf("�˺��Ѵ��ļ� %s ���ء�\n", ACCOUNTS_FILE);
    fclose(file);
}
//*****************************************************************************************************************************************************//

//*************************************�˻��������湦�ܺ���***************************************************************************************************//
// ����˺��û����Ƿ��Ѵ��ڵĺ���
bool usernameExists(const char* username) {
    for (int i = 0; i < numAccounts; i++) {
        if (strcmp(accounts[i].username, username) == 0) {
            return true;
        }
    }
    return false;
}

// �����˺ŵĺ���
void creatAccount(const char* username, const char* password) {
    if (usernameExists(username)) {
        printf("�û����Ѵ��ڡ���ʹ�ò�ͬ���û�����\n");
        printf("�������˵�\n");
        return;
    }
    if (numAccounts < MAX_ACCOUNTS) {
        strcpy(accounts[numAccounts].username, username);
        strcpy(accounts[numAccounts].password, password);
        numAccounts++;
        printf("�˺Ŵ����ɹ���\n");
        saveAccounts(); // �Զ������˺�
        printf("�������˵�\n");
    } else {
        printf("�޷���Ӹ����˺š��Ѵﵽ������ơ�\n");
        printf("�������˵�\n");
    }
}

// �����˺ŵ��˻���������
void addAccount(){
    printf("\n");
    printf("\t\t     ======================    \n");
    printf("\t\t             �����˺�      \n");
    printf("\t\t     ======================    \n\n");
    char username[NUMBER];
    char password[NUMBER];

    printf("��������Ҫ�������û�����");
    scanf("%s", username);
    printf("��������Ҫ���������룺");
    scanf("%s", password);

    creatAccount(username, password);
}


// ��֤�˺��Ƿ���ȷ�ĺ���
bool verifyCredentials(const char* username, const char* password) {
    for (int i = 0; i < numAccounts; i++) {
        if (strcmp(accounts[i].username, username) == 0 && strcmp(accounts[i].password, password) == 0) {
            return true; // ��֤�ɹ�
        }
    }
    return false; // ��֤ʧ��
}

// ��ʾ�����˺ţ���һ��Ϊ����Ա�˺�
void displayAccounts() {
    if (numAccounts == 0) {
        printf("û���˺š�\n");
        return;
    }

    for (int i = 0; i < numAccounts; i++) {
        if (i == 0) {
            printf("�˺� %d: %s (����Ա)\n", i + 1, accounts[i].username);
        } else {
            printf("�˺� %d: %s\n", i + 1, accounts[i].username);
        }
    }
    printf("����������ء�����\n");
    getch();
}


// ɾ��ָ���˺ţ����˵�һ������Ա�˺ţ�
void deleteAccount(const char* username) {
    if (strcmp(username, accounts[0].username) == 0) {
        printf("����ɾ������Ա�˺š�\n");
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
        printf("δ�ҵ��˺� %s��\n", username);
        return;
    }

    for (int i = index; i < numAccounts - 1; i++) {
        accounts[i] = accounts[i + 1];
    }
    numAccounts--;
    saveAccounts();
    printf("�˺� %s ��ɾ����\n", username);
    printf("����������ء�����\n");
    getch();
}

// ɾ�������˺ţ����˵�һ������Ա�˺ţ�
void deleteAllAccounts() {
    if (numAccounts <= 1) {
        printf("û�п�ɾ�����˺š�\n");
        return;
    }

    numAccounts = 1; // ֻ������һ������Ա�˺�
    saveAccounts();
    printf("�����˺���ɾ�������˹���Ա�˺ţ���\n");
    printf("����������ء�����\n");
    getch();
}


// �޸�ָ���˺�����ĺ���
void changePassword() {
    printf("\n");
    printf("\t\t     ======================    \n");
    printf("\t\t         ��ӭ�����޸�����      \n");
    printf("\t\t     ======================    \n");
    printf("\n");

    char username[NUMBER];
    char password[NUMBER];
    char new_password1[NUMBER];   //��ŵ�һ��������
    char new_password2[NUMBER];  //��ŵڶ���������

    printf("������ԭ�˺��û�����");
    scanf("%s", username);
    printf("������ԭ�˺����룺");
    scanf("%s", password);

    if (!verifyCredentials(username, password)) {
        printf("�û�����������󡣾ܾ����ʡ�\n");
        return;
    }else{
        printf("�˺�������ȷ�������޸�����\n");
    }

    printf("�����������룺\n");
    scanf("%s",&new_password1);
    printf("������һ�Σ�\n");
    scanf("%s",&new_password2);
    if(!strcmp(new_password1,new_password2)) //�Ƚ� �����޸ĺ����������
    {
        for (int i = 0; i < numAccounts; i++) {
            if (strcmp(accounts[i].username, username) == 0) {
                strcpy(accounts[i].password, new_password2);
                printf("�����޸ĳɹ�;�˺ţ�%s; �����룺%s; ���μǣ���\n", username,new_password2);
                saveAccounts(); // �Զ������˺�
                printf("����������ء�����\n");
                getch();
                return;
            }
        }
    }
    else
    {
        printf("����������������벻һ�£�����������ء�����\n");
        getch();
        return;
    }
}
// ����Աģʽ
void Administrator(BookList* list) {
    printf("\n");
    printf("\t\t     ======================    \n");
    printf("\t\t         ��ӭʹ�ù���Աģʽ      \n");
    printf("\t\t     ======================    \n");
    printf("���¼��\n");

    char username[NUMBER];
    char password[NUMBER];

    printf("���������Ա�û�����");
    scanf("%s", username);
    printf("���������Ա���룺");
    scanf("%s", password);

    if ((!strcmp(password,accounts[0].password)) && (!strcmp(username,accounts[0].username))) {
        printf("�˺�������ȷ���������Ա�˵�\n");
    }else{
        printf("�û�����������󡣾ܾ����ʡ�\n");
        return;
    }

    int choice;
    do {
        printf("\n");
        printf("*****************************************\n");
        printf("*           ---- ����Ա�˵� ----          *\n");
        printf("*****************************************\n");
        printf("****************�鼮�޸Ĳ���***************\n");
        printf("* 1. ����鼮                             *\n");
        printf("* 2. ɾ���鼮������Ż�������                *\n");
        printf("* 3. �����鼮״̬������Ż�������             *\n");
        printf("* 4. �����鼮������Ϣ������Ż�������          *\n");
        printf("****************�鼮��ʾ����***************\n");
        printf("* 5. ����Ų��ҵ����鼮                     *\n");
        printf("* 6. ���������ҵ����鼮                     *\n");
        printf("* 7. ��������ʾ�����鼮                     *\n");
        printf("* 8. ���ڹ�״̬��ʾ�����鼮                  *\n");
        printf("* 9. �������ʾ�����鼮                     *\n");
        printf("* 10. ɾ�������鼮                         *\n");
        printf("****************�˺��޸Ĳ���****************\n");
        printf("* 11. ��ʾ�����˺�                         *\n");
        printf("* 12. ɾ��ָ���˺ţ�����Ա�˺��޷�ɾ����       *\n");
        printf("* 13. ɾ�������˺ţ�����Ա�˺��޷�ɾ����       *\n");
        printf("* 14. �޸Ĺ���Ա�˺�����                    *\n");
        printf("* 15. �����Ķ��ƻ�                         *\n");
        printf("* 16. �������˵�                           *\n");
        printf("******************************************\n");
        printf("��ѡ�����Ĳ���: ");
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
                printf("�������鼮��Ϣ����� ���� ���� ״̬ �Ķ���ʱ��: ");
                scanf("%d %s %s %s %f", &id, name, category, status, &readTime);
                addBook(list, id, name, category, status, readTime);
                quickSort(list);
                saveBooks(list);
                printf("����������ء�����\n");
                getch();
                break;
            case 2:
                printf("�������鼮��ţ���֪������0�����鼮���ƣ���֪������NULL�����ո������: ");
                scanf("%d %s", &id, name);
                deleteBook(list, id, name);
                break;
            case 3:
                printf("�������鼮��ţ���֪������0�����鼮���ƣ���֪������NULL�����ո������: ");
                scanf("%d %s", &id, name);
                printf("�������鼮��״̬: ");
                scanf("%s", status);
                updateBookStatus(list, id, name, status);
                break;
            case 4:
                updateBook(list);
                break;
            case 5:
                printf("������Ҫ���ҵ��鼮���: ");
                scanf("%d", &id);
                book = findBookById(list, id);
                if (book != NULL) {
                    printf("�ҵ�ָ����ŵ��鼮��%s; ��ţ�%d; ���ࣺ%s; ״̬��%s; �Ķ���ʱ��%.1f��\n", book->name, book->id, book->category, book->status, book->readTime);
                } else {
                    printf("\nδ�ҵ�ָ����ŵ��鼮��\n");
                }
                printf("����������ء�����\n");
                getch();
                break;
            case 6:
                printf("������Ҫ���ҵ��鼮����: ");
                scanf("%s", name);
                book = findBookByName(list, name);
                if (book != NULL) {
                    printf("�ҵ�ָ���������鼮��%s; ��ţ�%d; ���ࣺ%s; ״̬��%s; �Ķ���ʱ��%.1f��\n", book->name, book->id, book->category, book->status, book->readTime);
                } else {
                    printf("\nδ�ҵ�ָ���������鼮��\n");
                }
                printf("����������ء�����\n");
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
                printf("������Ҫɾ�����˺��û���: ");
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
                printf("�������˵�\n");
                break;
            default:
                printf("��Ч��ѡ�������ѡ��\n");
        }
    } while (choice != 16);
}
// ��ͨ�û�ģʽ
void UserInterface(BookList* list) {
    printf("\n");
    printf("\t\t     ======================    \n");
    printf("\t\t        ��ӭʹ���û�ģʽ        \n");
    printf("\t\t     ======================    \n");
    printf("���¼��\n");

    char username[NUMBER];
    char password[NUMBER];

    printf("�������û�����");
    scanf("%s", username);
    printf("���������룺");
    scanf("%s", password);

    if (!verifyCredentials(username, password)) {
        printf("�û�����������󡣾ܾ����ʡ�\n");
        return;
    }

    printf("�˺�������ȷ�������û��˵�\n");

    int choice;
    do {
        printf("\n");
        printf("*****************************************\n");
        printf("*           ---- �û��˵� ----            *\n");
        printf("*****************************************\n");
        printf("* 1. ����Ų��ҵ����鼮                     *\n");
        printf("* 2. ���������ҵ����鼮                     *\n");
        printf("* 3. ��������ʾ�����鼮                     *\n");
        printf("* 4. ���ڹ�״̬��ʾ�����鼮                  *\n");
        printf("* 5. �������ʾ�����鼮                     *\n");
        printf("* 6. �����鼮                             *\n");
        printf("* 7. �����Ķ��ƻ�                          *\n");
        printf("* 8. �޸��˺�����                          *\n");
        printf("* 9. �������˵�                           *\n");
        printf("*****************************************\n");
        printf("��ѡ�����Ĳ���: ");
        scanf("%d", &choice);

        int id;
        char name[NUMBER];
        Book* book = NULL;

        switch (choice) {
            case 1:
                printf("������Ҫ���ҵ��鼮���: ");
                scanf("%d", &id);
                book = findBookById(list, id);
                if (book != NULL) {
                    printf("�ҵ�ָ����ŵ��鼮��%s; ��ţ�%d; ���ࣺ%s; ״̬��%s; �Ķ���ʱ��%.1f��\n", book->name, book->id, book->category, book->status, book->readTime);
                } else {
                    printf("\nδ�ҵ�ָ����ŵ��鼮��\n");
                }
                printf("����������ء�����\n");
                getch();
                break;
            case 2:
                printf("������Ҫ���ҵ��鼮����: ");
                scanf("%s", name);
                book = findBookByName(list, name);
                if (book != NULL) {
                    printf("�ҵ�ָ���������鼮��%s; ��ţ�%d; ���ࣺ%s; ״̬��%s; �Ķ���ʱ��%.1f��\n", book->name, book->id, book->category, book->status, book->readTime);
                } else {
                    printf("\nδ�ҵ�ָ���������鼮��\n");
                }
                printf("����������ء�����\n");
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
                printf("�������˵�\n");
                break;
            default:
                printf("��Ч��ѡ�������ѡ��\n");
        }
    } while (choice != 9);
}

// ���˵�
void main_menu() {
    BookList list;
    initList(&list);

    loadAccounts(); // ���ı��ļ������˺���Ϣ
    loadBooks(&list); // ���ı��ļ������鼮��Ϣ

    // ��ӳ�ʼ�鼮��Ϣ
//    addBook(&list, 1, "��ѧ����", "�̿���", "�ڹ�", 30);
//    addBook(&list, 11, "ʷ��", "��ʷ", "�ڹ�", 15);
//    addBook(&list, 12, "����ͨ��", "��ʷ", "�ڽ�", 27);
//    addBook(&list, 21, "�ʱ���", "����", "�ڹ�", 25);
//    addBook(&list, 31, "��ķ����", "��ѧ", "�ڹ�", 1);
//    addBook(&list, 36, "������ʱ��", "��ѧ", "�ڹ�", 6);
//    addBook(&list, 41, "������Ϣϵͳ����", "����", "�ڹ�", 3);
//    addBook(&list, 46, "����ԭ��", "��ѧ", "�ڹ�", 3);
//    addBook(&list, 51, "��������磺�Զ����·���", "����", "�ڽ�", 18);
//    addBook(&list, 56, "���Ͻ���ѧ", "ҽѧ", "�ڹ�", 43);
//    addBook(&list, 61, "�����Ĺ���", "����", "�ڹ�", 7);
//    addBook(&list, 66, "�ڽ̵Ľ���", "�ڽ�", "�ڹ�", 5);
//    addBook(&list, 71, "ֲ������ԭ��", "ũѧ", "�ڹ�", 2);
//    addBook(&list, 76, "�����ߵľ���", "����", "�ڹ�", 0.5);
//    addBook(&list, 81, "С����", "��ͯ", "�ڽ�", 0.5);

    // ��Ӷ����鼮��Ϣ��ÿһ���鼮�����������
//    addBook(&list, 2, "����ѧԭ��", "�̿���", "�ڹ�", 25);
//    addBook(&list, 9, "�������ۻ���", "�̿���", "�ڹ�", 24);
//    addBook(&list, 10, "��·��������", "�̿���", "�ڹ�", 27);
//    addBook(&list, 13, "�й�ͨʷ", "��ʷ", "�ڽ�", 23);
//    addBook(&list, 14, "����ʷ��Ҫ", "��ʷ", "�ڽ�", 19);
//    addBook(&list, 16, "�Ŵ�����ʷ", "��ʷ", "�ڽ�", 17);
//    addBook(&list, 28, "�������Ƹĸ�", "����", "�ڹ�", 24);
//    addBook(&list, 29, "������ѧ����", "����", "�ڹ�", 27);
//    addBook(&list, 30, "���÷�ѧ", "����", "�ڹ�", 29);
//    addBook(&list, 33, "��¥��", "��ѧ", "�ڹ�", 10);
//    addBook(&list, 34, "�˼�ʧ��", "��ѧ", "�ڹ�", 7);
//    addBook(&list, 35, "����¶�", "��ѧ", "�ڹ�", 3);
//    addBook(&list, 37, "������ѧʷ", "��ѧ", "�ڹ�", 8);
//    addBook(&list, 39, "�߼�ѧ����", "��ѧ", "�ڹ�", 12);
//    addBook(&list, 40, "������ѧ", "��ѧ", "�ڹ�", 15);
//    addBook(&list, 43, "�����ѧ����", "����", "�ڹ�", 8);
//    addBook(&list, 44, "��Ȼ�ֺ�ѧ", "����", "�ڹ�", 10);
//    addBook(&list, 45, "����ѧԭ��", "����", "�ڹ�", 5);
//    addBook(&list, 48, "���۵���", "��ѧ", "�ڹ�", 4);
//    addBook(&list, 49, "����ѧ����", "��ѧ", "�ڹ�", 7);
//    addBook(&list, 50, "�����۵���", "��ѧ", "�ڹ�", 6);
//    addBook(&list, 53, "������̵���", "����", "�ڽ�", 20);
//    addBook(&list, 54, "���ݿ�ϵͳԭ��", "����", "�ڽ�", 22);
//    addBook(&list, 55, "�������ԭ��", "����", "�ڽ�", 16);
//    addBook(&list, 57, "�ڿ�ѧ�̳�", "ҽѧ", "�ڹ�", 40);
//    addBook(&list, 58, "���ѧ����", "ҽѧ", "�ڹ�", 45);
//    addBook(&list, 59, "ҩ��ѧ����", "ҽѧ", "�ڹ�", 38);
//    addBook(&list, 63, "�滭��������������", "����", "�ڹ�", 5);
//    addBook(&list, 64, "�������������", "����", "�ڹ�", 9);
//    addBook(&list, 65, "���ּ�������ʷ", "����", "�ڹ�", 8);
//    addBook(&list, 68, "��˹����ʷ", "�ڽ�", "�ڹ�", 4);
//    addBook(&list, 69, "��������������ʷ", "�ڽ�", "�ڹ�", 6);
//    addBook(&list, 70, "ӡ�Ƚ��Ļ����ڽ�", "�ڽ�", "�ڹ�", 3);
//    addBook(&list, 73, "�����Ŵ�����", "ũѧ", "�ڹ�", 4);
//    addBook(&list, 74, "԰��ֲ��ѧ", "ũѧ", "�ڹ�", 3);
//    addBook(&list, 75, "ũ������չ�滮", "ũѧ", "�ڹ�", 6);
//    addBook(&list, 78, "�Ŷӽ��������", "����", "�ڹ�", 0.7);
//    addBook(&list, 79, "�г�Ӫ������", "����", "�ڹ�", 1.2);
//    addBook(&list, 80, "��Ŀ����ʵ��", "����", "�ڹ�", 0.8);
//    addBook(&list, 83, "����ͯ��", "��ͯ", "�ڽ�", 0.5);
//    addBook(&list, 84, "����������ħ��ʯ", "��ͯ", "�ڽ�", 1.5);
//    addBook(&list, 85, "³��ѷƯ����", "��ͯ", "�ڽ�", 1);



    // ���鼮����Ž�������
    quickSort(&list);
    saveBooks(&list);

    printf("\n\n");
    printf("*****************************************\n");
    printf("*    ��ӭ�������ο���ͼ����ļƻ�����ϵͳ      *\n");
    printf("*****************************************\n");
    printf("\n");

    int choice;
    do {
        menu:
        printf("\n");
        printf("*****************************************\n");
        printf("*           ---- ���˵� ----             *\n");
        printf("*****************************************\n");
        printf("* 1. ����Ա��¼                           *\n");
        printf("* 2. ��ͨ�û���¼                          *\n");
        printf("* 3. �޸�ָ���˺ŵ�����                     *\n");
        printf("* 4. �����˺�                             *\n");
        printf("* 5. �˳�                                *\n");
        printf("*****************************************\n");
        printf("��ѡ�����Ĳ���: ");
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
                printf("�˳�ϵͳ\n");
                break;
            default:
                printf("��Ч��ѡ�������ѡ��\n");
        }
    } while (choice != 5);
}
//*****************************************************************************************************************************************************//

int main() {

    main_menu();

    return 0;
}

