#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#define MAX 100

typedef struct inforWeb
{
    char name[MAX];
    char url[MAX];
    char accessTime[MAX];
} inforWeb;

typedef struct Stack
{
    inforWeb *historyWeb[MAX];
    int top;
} Stack;

void createInitStack(Stack *stack)
{
    stack->top = -1;
}

int isEmpty(Stack *stack)
{
    return stack->top == -1;
}

int isFull(Stack *stack)
{
    return stack->top >= MAX - 1;
}

void pushToHistory(Stack *stack, inforWeb *web)
{
    if (isFull(stack))
    {
        printf("Ngan xep day\n");
        return;
    }
    inforWeb *newWeb = (inforWeb *)malloc(sizeof(inforWeb));
    if (newWeb == NULL)
    {
        perror("Loi cap phat bo nho");
        exit(1);
    }
    *newWeb = *web;
    stack->historyWeb[++stack->top] = newWeb;
}

void createWeb(inforWeb *web, char name[MAX])
{
    time_t now = time(0);
    char *dt = ctime(&now);
    dt[strcspn(dt, "\n")] = 0;
    strncpy(web->name, name, MAX - 1);
    snprintf(web->url, MAX, "https://www.%s", name);
    strncpy(web->accessTime, dt, MAX - 1);
}

void printHistoryFromFile(const char *filename)
{
    FILE *f = fopen(filename, "r");
    if (f == NULL)
    {
        perror("Loi mo file");
        return;
    }
    char line[MAX * 3];
    while (fgets(line, sizeof(line), f) != NULL)
    {
        printf("%s", line);
    }
    fclose(f);
}

void goBackToPreviousPage(Stack *stack)
{
    if (isEmpty(stack))
    {
        printf("Khong co trang truoc de quay lai.\n");
    }
    else
    {
        inforWeb *prevWeb = stack->historyWeb[stack->top--];
        printf("Da quay lai trang truoc: Name: %s, URL: %s, Access Time: %s\n",
               prevWeb->name, prevWeb->url, prevWeb->accessTime);
        free(prevWeb);
    }
}
void printWebNow(Stack *stack)
{
    if (isEmpty(stack))
    {
        printf("Trang web hien tai trong.\n");
        return;
    }
    printf("Trang web hien tai: %s, URL: %s, Access Time: %s\n", stack->historyWeb[stack->top]->name, stack->historyWeb[stack->top]->url, stack->historyWeb[stack->top]->accessTime);
}
void freeStack(Stack *stack)
{
    while (!isEmpty(stack))
    {
        free(stack->historyWeb[stack->top--]);
    }
}

void deleteWebFromHistory(Stack *stack, const char *nameToDelete, const char *filename)
{
    if (isEmpty(stack))
    {
        printf("Lich su trong, khong co trang nao de xoa.\n");
        return;
    }

    int found = 0;

    FILE *tempFile = fopen(filename, "w");
    if (tempFile == NULL)
    {
        perror("Loi mo file");
        return;
    }

    Stack tempStack;
    createInitStack(&tempStack);

    for (int i = 0; i <= stack->top; i++)
    {
        inforWeb *currentWeb = stack->historyWeb[i];
        if (strcmp(currentWeb->name, nameToDelete) == 0)
        {
            printf("Da xoa trang web: %s\n", currentWeb->name);
            free(currentWeb); 
            found = 1;
        }
        else
        {
            pushToHistory(&tempStack, currentWeb);
            fprintf(tempFile, "Name: %s, URL: %s, Access Time: %s\n",
                    currentWeb->name, currentWeb->url, currentWeb->accessTime);
        }
    }

    fclose(tempFile);

    createInitStack(stack); 
    while (!isEmpty(&tempStack))
    {
        stack->historyWeb[++stack->top] = tempStack.historyWeb[tempStack.top--];
    }

    if (!found)
    {
        printf("Khong tim thay trang web: %s\n", nameToDelete);
    }
}



int main()
{
    inforWeb web;
    Stack history;
    createInitStack(&history);
    char name[MAX];
    int choice;
    FILE *f;

    f = fopen("historyWeb.txt", "a");
    if (f == NULL)
    {
        perror("Loi mo file");
        return 1;
    }
    do
    {
        printf("\n----------MENU----------\n");
        printf("1. Nhap ten trang web muon truy cap\n");
        printf("2. Quay lai trang truoc do\n");
        printf("3. Xem trang hien tai\n");
        printf("4. Hien thi toan bo lich su\n");
        printf("5. Xoa lich su web\n");
        printf("6. Thoat\n");
        printf("Nhap lua chon cua ban: ");
        scanf("%d", &choice);
        getchar();
        switch (choice)
        {
        case 1:
            printf("Nhap ten trang web muon truy cap: ");
            if (fgets(name, MAX, stdin) == NULL)
            {
                perror("Loi nhap du lieu");
                break;
            }
            name[strcspn(name, "\n")] = 0;
            createWeb(&web, name);
            pushToHistory(&history, &web);
            fprintf(f, "Name: %s, URL: %s, Access Time: %s\n", web.name, web.url, web.accessTime);
            fflush(f);
            break;
        case 2:
            goBackToPreviousPage(&history);
            break;
        case 3:
            printf("Trang web hien tai: \n");
            printWebNow(&history);
            break;
        case 4:
            printf("Lich su web: \n");
            printHistoryFromFile("historyWeb.txt");
            break;
        case 5:
		    printf("Nhap ten trang web muon xoa: ");
		    if (fgets(name, MAX, stdin) == NULL)
		    {
		        perror("Loi nhap du lieu");
		        break;
		    }
		    name[strcspn(name, "\n")] = 0;
		    deleteWebFromHistory(&history, name, "historyWeb.txt");
		    break;
        case 6:
            printf("Cam on ban da su dung chuong trinh\n");
            break;
        default:
            printf("Lua chon khong hop le\n");
            break;
        }
    } while (choice != 6);
    fclose(f);
    freeStack(&history);
    return 0;
}
