lborder = ? ;
rborder = ? ;
if (rborder <= lborder) {
  print(1);
}
while (rborder != lborder) {
  num = lborder;
  copy = lborder;
  isprint = 1;
  while (copy > 0) {
    div = copy % 10;
    if (div) {
      if (div && num % div != 0) {
        isprint = 0;
      }
    } else {
      isprint = 0;
    }
    copy = copy / 10;
  }
  if (isprint) {
    print(num);
  }
  ++lborder;
}
