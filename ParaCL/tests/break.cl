i = j = 0;
while(++i <= 10){
    if(i==4 || i==6) {
        continue;
    }
    while(++j<=10) {
        if(i==j)
        {
            print(i);
            break;
        }
    }
}