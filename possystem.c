#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<termios.h>
#include<unistd.h>

#define MAX_ITEM 100
#define MAX_USER 50
#define MAX_SALE 100
#define MAX_BATCH 20
typedef struct{
    int stock;          // 이 입고 묶음의 수량
    time_t input_time;  // 이 입고 묶음의 입고 시간
} Batch;

typedef struct{
    char itemid[50];
    int price;
    int stock;
    int expire_time;
    int is_adult;
    
    Batch batches[MAX_BATCH];
    int batch_count;
} Item;

typedef struct{
    char userid[50];
    char password[50];
    char username[50];
}User;

typedef struct{
    char itemid[50];
    int count;
    int price;
    int type;       // 1: 판매, 2: 환불
} Sale;

Item items[MAX_ITEM];
int item_count = 0;

User users[MAX_USER];
int user_count = 0;

Sale sales[MAX_SALE];
int sale_count = 0;

time_t login_time;

long total_sales = 390500;
long one_sales = 0;

//users.csv파일 불러오기
void load_users()
{
    FILE *fp = fopen("users.csv", "r");
    char line[200];
    char *token;
    if(fp == NULL)
    {
        printf("users.csv 파일을 읽을 수 없습니다.\n");
        return;
    }

    fgets(line, sizeof(line), fp); //첫줄 건너뛰기

    //,단위로 잘라내기
    while(fgets(line, sizeof(line), fp) != NULL)
    {
        token = strtok(line, ",\n");
        if(token == NULL)
            continue;
        strcpy(users[user_count].userid, token);
        token = strtok(NULL, ",\n");
        if(token == NULL)
            continue;
        strcpy(users[user_count].password, token);
        token = strtok(NULL, ",\n");
        if(token == NULL)
            continue;
        strcpy(users[user_count].username, token);

        user_count++;
    }

    fclose(fp);
}
//items.csv 불러오기
void load_items()
{
    FILE *fp = fopen("items.csv", "r");
    char line[200];
    char *token;

    if(fp == NULL) {
        printf("items.csv 파일을 읽을 수 없습니다.");
        return;
    }   
    fgets(line, sizeof(line), fp); // 첫줄 건너뛰기

    while(fgets(line, sizeof(line), fp) != NULL)
    {

         token = strtok(line, ",");
         if(token == NULL)
            continue;
         strcpy(items[item_count].itemid, token);
         token = strtok(NULL, ",");
         items[item_count].price = atoi(token);
         token = strtok(NULL, ",");
         items[item_count].stock = atoi(token);
         token = strtok(NULL, ",");
         items[item_count].expire_time = atoi(token);
         token = strtok(NULL, ",");
         items[item_count].is_adult = atoi(token);
         items[item_count].batch_count = 0;
        item_count++;
    }
   
    fclose(fp);
}
// users.csv 목록에서 몇번째에 있는지 확인하는 함수
int find_user(char *userid)
{
    for(int i = 0; i < user_count; i++)
    {
        if(strcmp(users[i].userid, userid) == 0)
        {
            return i;
        }
    }

    return -1;
}
//변경된 정보를  users.csv 에 다시 저장하는 함수 
void save_users()
{
    FILE *fp = fopen("users.csv", "w");

    if(fp == NULL)
    {
        printf("users.csv 파일을 저장할 수 없습니다.\n");
        return;
    }

    fprintf(fp, "userid,password,username\n");

    for(int i = 0; i < user_count; i++)
    {
        fprintf(fp, "%s,%s,%s\n", users[i].userid, users[i].password, users[i].username);
    }

    fclose(fp);
}
//숫자이외에 다른 걸 입력시 다시 입력하라는 함수
int input_number()
{
    int num;
    int result;

    while(1)
    {
        result = scanf("%d", &num);

        if(result == 1)
        {
            while(getchar() != '\n'); //버퍼 비우기
            return num;
        }

        printf("숫자만 입력해주세요 :");

        // 잘못 입력된 문자 버리기
        while(getchar() != '\n');
    }
}
//패스워드 가리는 함수
void input_password(char *password)
{
    struct termios oldt, newt;
    int i = 0;
    char ch;

    tcgetattr(STDIN_FILENO, &oldt);

    newt = oldt;
    newt.c_lflag &= ~(ECHO | ICANON);

    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    while(1)
    {
        ch = getchar();

        if(ch == '\n')
            break;

        // 백스페이스 처리
        if(ch == 127 || ch == '\b')
        {
            if(i > 0)
            {
                i--;

                printf("\b \b");
                fflush(stdout);
            }

            continue;
        }

        if(i < 49)
        {
            password[i] = ch;
            i++;

            printf("*");
            fflush(stdout);
        }
    }

    password[i] = '\0';

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    printf("\n");
}
//사용자 추가하는 함수
void add_user()
{
    char id[50];
    char pw[50];
    char name[50];

     if(user_count >= MAX_USER)
    {
        printf("더 이상 사용자를 등록할 수 없습니다.\n");
        return;
    }

    printf("\n===== 사용자 등록 =====\n");

    printf("아이디 : ");
    scanf("%49s", id);

    if(find_user(id) != -1)
    {
        printf("이미 존재하는 아이디입니다.\n");
        return;
    }
    getchar();

    printf("비밀번호 : ");
    input_password(pw);

    printf("사용자 이름 : ");
    scanf("%49s", name);

    strcpy(users[user_count].userid, id);
    strcpy(users[user_count].password, pw);
    strcpy(users[user_count].username, name);

    user_count++;

    save_users();

    printf("사용자 등록 완료\n");
}
// items.csv 목록에서 몇번째에 있는지 확인하는 함수
int find_item(char *itemid)
{
    for(int i = 0; i < item_count; i++)
    {
        if(strcmp(items[i].itemid, itemid) == 0)
        {
            return i;
        }
    }

    return -1;
}

//변경된 상품 정보를  items.csv 에 다시 저장하는 함수 
void save_items()
{
    FILE *fp = fopen("items.csv", "w");

    if(fp == NULL)
    {
        printf("파일을 저장할 수 없습니다.\n");
        return;
    }

    fprintf(fp, "itemid,price,stock,expire_time,is_adult\n");

    for(int i = 0; i < item_count; i++)
    {
        fprintf(fp, "%s,%d,%d,%d,%d\n",
                items[i].itemid,
                items[i].price,
                items[i].stock,
                items[i].expire_time,
                items[i].is_adult);
    }

    fclose(fp);
}
//변경된 정보를  batches.csv 에 다시 저장하는 함수 
void save_batches()
{
    FILE *fp = fopen("batches.csv", "w");

    if(fp == NULL)
    {
        printf("batches.csv 파일을 저장할 수 없습니다.\n");
        return;
    }

    fprintf(fp, "itemid,stock,input_time\n");

    for(int i = 0; i < item_count; i++)
    {
        for(int j = 0; j < items[i].batch_count; j++)
        {
            if(items[i].batches[j].stock > 0)
            {
                fprintf(fp, "%s,%d,%ld\n",
                        items[i].itemid,
                        items[i].batches[j].stock,
                        (long)items[i].batches[j].input_time);
            }
        }
    }

    fclose(fp);
}
// batches.csv 목록에서 몇번째에 있는지 확인하는 함수
void load_batches()
{
    FILE *fp = fopen("batches.csv", "r");

    if(fp == NULL)
        return;

    char line[200];
    char name[50];
    int stock;
    long input_time;

    fgets(line, sizeof(line), fp); // 첫 줄 건너뛰기

    // 유통기한 상품의 재고는 batch 기준으로 다시 계산
    for(int i = 0; i < item_count; i++)
    {
        items[i].batch_count = 0;

        if(items[i].expire_time > 0)
            items[i].stock = 0;
    }

    while(fgets(line, sizeof(line), fp) != NULL)
    {
        if(sscanf(line, "%49[^,],%d,%ld", name, &stock, &input_time) == 3)
        {
            int index = find_item(name);

            if(index != -1 && items[index].batch_count < MAX_BATCH)
            {
                int b = items[index].batch_count;

                items[index].batches[b].stock = stock;
                items[index].batches[b].input_time = (time_t)input_time;

                items[index].batch_count++;
                items[index].stock += stock;
            }
        }
    }

    fclose(fp);
}
//20개가 최대인데 묶음별 물품이 0개일 경우 삭제
void clean_batches(int index)
{
    int write = 0;

    for(int i = 0; i < items[index].batch_count; i++)
    {
        if(items[index].batches[i].stock > 0)
        {
            items[index].batches[write] =
                items[index].batches[i];

            write++;
        }
    }

    items[index].batch_count = write;
}
//상품 유통기한 설정 함수
int get_expire_time(char *name)
{
    int check;
    int hour;

    if(strcmp(name, "우유") == 0)
        return 24;

    if(strcmp(name, "두부") == 0)
        return 24;

    if(strcmp(name, "오뎅") == 0)
        return 1;

    while(1)
    {
        printf("유통기한이 있는 상품입니까? (0: 없음 / 1: 있음) : ");
        check = input_number();

        if(check == 0 || check == 1)
        break;

        printf("0 또는 1만 입력해주세요.\n");
    }

    if(check == 1)
    {
        while(1)
        {
            printf("유통기한 시간 입력 : ");
            hour = input_number();
            
            if(hour > 0)
            return hour;
        
            printf("유통기한은 1시간 이상이어야 합니다.\n");
        }
    }
    return 0;
}
// 로그인 함수  
int login()
{
    char input_id[50];
    char input_pw[50];

    while(1)
    {
        printf("\n===== 로그인 =====\n");
        printf("아이디 : ");
        scanf("%49s", input_id);

        getchar();

        printf("비밀번호 : ");
        input_password(input_pw);

        for(int i = 0; i< user_count; i++)
        {
            if(strcmp(users[i].userid, input_id) == 0 && strcmp(users[i].password, input_pw) == 0)
            {
                login_time = time(NULL);
                printf("로그인 성공! %s님 환영합니다.\n", users[i].username);
                printf("로그인 시간 : %s", ctime(&login_time));
                return i;
            }
        }
        printf("아이디 또는 비밀번호가 틀렸습니다.\n");
        printf("다시 입력해주세요.\n");
    }
}
//(1) 재고현황 함수
void check_inventory()
{
    printf("\n===== 재고 현황 =====\n");

    for(int i = 0; i < item_count; i++)
    {
        printf("%s(%d원) : ", items[i].itemid,items[i].price);

        for(int j = 0; j < items[i].stock; j++)
        {
            printf("*");
        }

        printf(" (%d개)\n", items[i].stock);
    }
}
//(2) 현재 잔고 체크 함수
void check_balance()
{
    printf("현재 잔고 : %ld원\n", total_sales + one_sales);
}
//(3) 오늘 매출액 함수
void check_sales()
{
    printf("오늘 매출액 : %ld원\n", one_sales);
}
//(4)유통기한 물품 체크 함수
void check_expire()
{
    printf("\n===== 유통기한 관리 상품 =====\n");

    int found = 0;
    int changed = 0;
    time_t now = time(NULL);

    for(int i = 0; i < item_count; i++)
    {
        if(items[i].expire_time <= 0)
            continue;

        for(int j = 0; j < items[i].batch_count; j++)
        {
            if(items[i].batches[j].stock <= 0)
                continue;

            long passed = (now - items[i].batches[j].input_time) / 3600;

            long remain = items[i].expire_time - passed;

            found = 1;

            if(remain > 0)
            {
                printf("%s - %d차 입고 - 재고 %d개 - 남은 유통기한 %ld시간\n",
                       items[i].itemid,
                       j + 1,
                       items[i].batches[j].stock,
                       remain);
            }
            else
            {
                printf("%s - %d차 입고 - 유통기한 만료\n",
                       items[i].itemid,
                       j + 1);

                items[i].stock -= items[i].batches[j].stock;
                items[i].batches[j].stock = 0;

                changed = 1;
            }
        }
        clean_batches(i); 
    }

    if(found == 0)
    {
        printf("유통기한 관리 물품이 없습니다.\n");
    }

    if(changed == 1)
    {
        save_items();
        save_batches();
    }
}
// 성인 확인 하는 함수
int check_adult(int year, int month, int day)
{
    time_t now = time(NULL);
    struct tm *today = localtime(&now);
    int age = (today->tm_year + 1900) - year;

    if((today->tm_mon + 1 < month) || (today->tm_mon + 1 == month && today->tm_mday < day))
    {
        age--;
    }

    return age >= 19;
}
// (6)업무 종료 함수
void end_work(int login_user)
{
    char pw[50];
    
    while(1)
    {
        printf("비밀번호 : ");
        
        input_password(pw);

        if(strcmp(users[login_user].password, pw) == 0)
            break;

        printf("비밀번호가 틀렸습니다. 다시 입력해주세요.\n");
    }

    time_t logout_time = time(NULL);
    long running_time = logout_time - login_time;
    long wages = running_time * 9800 / 3600;

    printf("\n===== 업무 종료 =====\n");
    printf("사용자 : %s\n", users[login_user].username);
    printf("로그인 시간 : %s", ctime(&login_time));
    printf("근무 시간 : %ld시간 %ld분 %ld초\n", running_time / 3600, (running_time % 3600) / 60, running_time % 60);
    printf("오늘 매출액 : %ld원\n", one_sales);
    printf("현재 잔고 : %ld원\n", total_sales + one_sales);
    printf("오늘 임금 : %ld원\n", wages);
}

//(5).1 물건 판매 함수
void sell_item()
{
    char name[50];
    int count;
    int money;
    int year, month, day;

    printf("상품명 : ");
    scanf("%49s", name);

    int index = find_item(name);

    if(index == -1)
    {
        printf("없는 상품입니다.\n");
        return;
    }

    // 유통기한 상품의 만료된 묶음 제거
    if(items[index].expire_time > 0)
    {
        time_t now = time(NULL);

        for(int i = 0; i < items[index].batch_count; i++)
        {
            if(items[index].batches[i].stock <= 0)
                continue;

            long passed =
                (now - items[index].batches[i].input_time) / 3600;

            if(passed >= items[index].expire_time)
            {
                items[index].stock -=
                    items[index].batches[i].stock;

                items[index].batches[i].stock = 0;
            }
        }

        save_items();
        save_batches();
    }

    // 재고가 없는 경우
    if(items[index].stock <= 0)
    {
        printf("판매 가능한 재고가 없습니다.\n");
        return;
    }

    printf("수량 : ");
    count = input_number();

    if(count <= 0)
    {
        printf("수량을 잘못 입력했습니다.\n");
        return;
    }

    if(count > items[index].stock)
    {
        printf("재고가 부족합니다.\n");
        return;
    }

    // 성인 상품 확인
    if(items[index].is_adult == 1)
    {
        int birth;

        printf("생년월일 8자리 입력(YYYYMMDD) : ");
        birth = input_number();

        if(birth < 10000000 || birth > 99999999)
        {
            printf("생년월일을 8자리로 입력해주세요.\n");
            return;
        }

        year = birth / 10000;
        month = (birth / 100) % 100;
        day = birth % 100;

        if(month < 1 || month > 12)
        {
            printf("월은 1~12 사이로 입력해주세요.\n");
            return;
        }

        if(day < 1 || day > 31)
        {
            printf("일은 1~31 사이로 입력해주세요.\n");
            return;
        }

        if(check_adult(year, month, day) == 0)
        {
            printf("미성년자에게 판매할 수 없습니다.\n");
            return;
        }
    }

    int total_price = items[index].price * count;

    printf("가격 : %d원\n", total_price);

    printf("받은 돈 : ");
    money = input_number();

    if(money < total_price)
    {
        printf("금액이 부족합니다.\n");
        return;
    }

    // 거래내역 배열이 가득 찼는지 먼저 확인
    if(sale_count >= MAX_SALE)
    {
        printf("거래 내역을 더 이상 저장할 수 없습니다.\n");
        return;
    }

    // 유통기한 상품은 먼저 입고된 묶음부터 판매
    if(items[index].expire_time > 0)
    {
        int need = count;

        for(int i = 0; i < items[index].batch_count; i++)
        {
            if(items[index].batches[i].stock <= 0)
                continue;

            if(items[index].batches[i].stock >= need)
            {
                items[index].batches[i].stock -= need;
                need = 0;
                break;
            }
            else
            {
                need -= items[index].batches[i].stock;
                items[index].batches[i].stock = 0;
            }
        }
        clean_batches(index); 
    }

    // 전체 재고 감소
    items[index].stock -= count;

    one_sales += total_price;

    // 판매 내역 저장
    strcpy(sales[sale_count].itemid, items[index].itemid);
    sales[sale_count].count = count;
    sales[sale_count].price = total_price;
    sales[sale_count].type = 1;
    sale_count++;

    printf("거스름돈 : %d원\n", money - total_price);
    printf("판매 완료\n");
    clean_batches(index);

    save_items();
    save_batches();
}

//(5).2 물건 환불 함수
void refund_item()
{
    char name[50];
    int count;

    printf("환불 상품명 : ");
    scanf("%49s", name);

    int index = find_item(name);

    if(index == -1)
    {
        printf("없는 상품입니다.\n");
        return;
    }

    int sold = 0;

    for(int i = 0; i < sale_count; i++)
    {
        if(strcmp(sales[i].itemid, name) == 0)
        {
            if(sales[i].type == 1)
                sold += sales[i].count;

            if(sales[i].type == 2)
                sold -= sales[i].count;
        }
    }

    if(sold <= 0)
    {
        printf("판매 내역이 없습니다.\n");
        return;
    }

    printf("환불 수량 : ");
    count = input_number();

    if(count <= 0)
    {
        printf("수량을 잘못 입력했습니다.\n");
        return;
    }

    if(count > sold)
    {
        printf("판매한 수량보다 많이 환불할 수 없습니다.\n");
        printf("환불 가능 수량 : %d개\n", sold);
        return;
    }

    // 거래내역 공간 먼저 확인
    if(sale_count >= MAX_SALE)
    {
        printf("거래 내역을 더 이상 저장할 수 없습니다.\n");
        return;
    }

    // 유통기한 상품이면 새로운 입고 묶음으로 처리
    if(items[index].expire_time > 0)
    {
        clean_batches(index);

        if(items[index].batch_count >= MAX_BATCH)
        {
            printf("입고 묶음을 더 이상 저장할 수 없습니다.\n");
            return;
        }

        int b = items[index].batch_count;

        items[index].batches[b].stock = count;
        items[index].batches[b].input_time = time(NULL);

        items[index].batch_count++;
    }

    // 전체 재고 증가
    items[index].stock += count;

    // 매출 감소
    one_sales -= items[index].price * count;

    // 환불 내역 저장
    strcpy(sales[sale_count].itemid, items[index].itemid);
    sales[sale_count].count = count;
    sales[sale_count].price = items[index].price * count;
    sales[sale_count].type = 2;

    sale_count++;

    printf("환불 수량 : %d개\n", count);
    printf("환불 금액 : %d원\n",
           items[index].price * count);

    printf("환불 완료\n");

    save_items();
    save_batches();
}
//(5).3 물품 입고 함수
void stock_item()
{
    char name[50];
    int index;
    int count;

    printf("상품명 : ");
    scanf("%49s", name);

    index = find_item(name);

   // 이미 존재하는 상품

   if(index != -1)
   {
        printf("이미 존재하는 상품입니다.\n");
        printf("입고 수량 : ");
        count = input_number();

        if(count <= 0)
        {
            printf("입고 수량을 잘못 입력했습니다.\n");
            return;
        }

    // 유통기한 상품일 때만 Batch 정리/제한 확인
        if(items[index].expire_time > 0)
        {
         clean_batches(index);

            if(items[index].batch_count >= MAX_BATCH)
            {
                printf("입고 묶음을 더 이상 저장할 수 없습니다.\n");
                return;
            }

            int b = items[index].batch_count;

            items[index].batches[b].stock = count;
            items[index].batches[b].input_time = time(NULL);
            items[index].batch_count++;
    }

    // 전체 재고 증가
        items[index].stock += count;

        save_items();
        save_batches();

        printf("입고 완료\n");
    }

    // 새로운 상품
    else
    {
        if(item_count >= MAX_ITEM)
        {
            printf("더 이상 상품을 추가할 수 없습니다.\n");
            return;
        }

        printf("새로운 상품입니다.\n");

        strcpy(items[item_count].itemid, name);

        printf("가격 : ");
        items[item_count].price = input_number();
        
        if(items[item_count].price <= 0)
        {
            printf("가격은 1원 이상이어야 합니다.\n");
            return;
        }
        
        printf("입고 수량 : ");
        items[item_count].stock = input_number();


        if(items[item_count].stock < 0)
        {
            printf("재고는 0개 이상이어야 합니다.\n");
            return;
        }
        printf("성인상품 여부(0: 일반 / 1: 성인상품) : ");
        items[item_count].is_adult = input_number();
        
        if(items[item_count].is_adult != 0 && items[item_count].is_adult != 1)
        {
            printf("성인상품 여부는 0 또는 1만 입력해주세요.\n");
            return;
        }
        
        items[item_count].expire_time = get_expire_time(name);
        items[item_count].batch_count = 0;
        if(items[item_count].expire_time > 0 && items[item_count].stock > 0)
        {
            items[item_count].batches[0].stock = items[item_count].stock;
            items[item_count].batches[0].input_time = time(NULL);
            items[item_count].batch_count = 1;
        }
        item_count++;
        save_items();
        save_batches();
        printf("입고 완료\n");
    }
}


//(5).4 물품 검색 함수
void search_item()
{
    char name[50];

    printf("검색할 상품명 : ");
    scanf("%49s", name);

    int index = find_item(name);

    if(index == -1)
    {
        printf("상품을 찾을 수 없습니다.\n");
        return;
    }

    printf("\n===== 상품 정보 =====\n");    printf("상품명 : %s\n", items[index].itemid);
    printf("가격 : %d원\n", items[index].price);
    printf("재고 : %d개\n", items[index].stock);
    printf("성인 상품 : %s\n", items[index].is_adult ? "성인상품 O" : "성인상품 X");
}
// (5).5 물품 삭제 함수
void delete_item()
{
    char name[50];

    printf("삭제할 상품명 : ");
    scanf("%49s", name);

    int index = find_item(name);

    if(index == -1)
    {
        printf("상품을 찾을 수 없습니다.\n");
        return;
    }

    // 삭제한 뒤 상품들을 한칸씩 앞으로 이동
    for(int i = index; i < item_count - 1; i++)
    {
        items[i] = items[i + 1];
    }

    item_count--;
    save_items();
    save_batches();
    printf("%s 상품이 삭제되었습니다.\n", name);
}
//(5).6 판매 /환불 내역
void show_sales()
{
    printf("\n===== 판매 / 환불 내역 =====\n");

    if(sale_count == 0)
    {
        printf("거래 내역이 없습니다.\n");
        return;
    }

    for(int i = 0; i < sale_count; i++)
    {
        if(sales[i].type == 1)
        {
            printf("[판매] ");
        }
        else
        {
            printf("[환불] ");
        }

        printf("%s / %d개 / %d원\n",sales[i].itemid, sales[i].count, sales[i].price);
    }
}
//(5). 업무 시작 함수
void work_menu()
{
    int menu;

    while(1)
    {
        printf("\n===== 업무 메뉴 =====\n");
        printf("1. 상품 판매\n");
        printf("2. 상품 환불\n");
        printf("3. 상품 입고\n");
        printf("4. 상품 검색\n");
        printf("5. 상품 삭제\n");
        printf("6. 판매/환불 내역\n");
        printf("7. 뒤로가기\n");
        printf("선택 : ");

        menu = input_number();

        switch(menu)
        {
            case 1:
                sell_item();
                break;
            case 2:
                refund_item();
                break;
            case 3:
                stock_item();
                break;
            case 4:
                search_item();
                break;
            case 5:
                delete_item();
                break;
            case 6:
                show_sales();
                break;
            case 7:
                return;
            default:
                printf("잘못된 입력입니다.\n");
        }
    }
}

int main()
{
    int start_menu;
    int menu;
    load_items();
    load_batches();
    load_users();
    
    while(1)
    {
        printf("\n===== POS 시스템 =====\n");
        printf("1. 로그인\n");
        printf("2. 사용자 등록\n");
        printf("선택 : ");
        start_menu = input_number();
        if(start_menu == 1)
        {
            break;
        }
        else if(start_menu == 2)
        {
            add_user();
        }
        else
        {
            printf("잘못된 입력입니다. 다시 입력해주세요.\n");
        }
    }
    int login_user = login();
    
    while(1)
     {
        printf("\n===== POS 메뉴 =====\n");
        printf("1. 재고 체크\n");
        printf("2. 현재 잔고 체크\n");
        printf("3. 매출액\n");
        printf("4. 유통기한 물품 체크\n");
        printf("5. 업무 시작\n");
        printf("6. 업무 종료\n");
        printf("선택 : ");

        menu = input_number();

        switch(menu)
        {
            case 1:
                check_inventory();
                break;
            case 2:
                check_balance();
                break;
            case 3:
                check_sales();
                break;
            case 4:
                check_expire();
                break;
            case 5:
                work_menu();
                break;
            case 6:
                end_work(login_user);
                return 0;
            default:
                printf("잘못된 입력입니다.\n");
        }
    }
    return 0;
}