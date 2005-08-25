#ifndef __TBE_INTERVAL_H
#define __TBE_INTERVAL_H

typedef struct {
  unsigned int ep_1;
  unsigned int ep_2;
  unsigned int ep_mask;
} tbe_interval;

/* masks for interval flags */
#define TBE_INTERVAL_EP_1   1
#define TBE_INTERVAL_EP_2   2
#define TBE_INTERVAL_EP_NUL 0
#define TBE_INTERVAL_EP_ALL (TBE_INTERVAL_EP_1 | TBE_INTERVAL_EP_2)

/* useful macros */
#define TBE_INTERVAL_EP_ISCLEAR(X) \
  (((X).ep_mask & TBE_INTERVAL_EP_ALL) == TBE_INTERVAL_EP_NUL)
#define TBE_INTERVAL_EP_ISFILL(X) \
  (((X).ep_mask & TBE_INTERVAL_EP_ALL) == TBE_INTERVAL_EP_ALL)
#define TBE_INTERVAL_EP_ISSET(X,Y) \
  (((X).ep_mask & (Y)) == (Y))

#define TBE_INTERVAL_ISVALID(X) \
  (TBE_INTERVAL_EP_ISFILL(X) ? ((X).ep_1 < (X).ep_2) : 1)
#define TBE_INTERVAL_INIT1(X) \
  (X).ep_1 = 0; (X).ep_2 = 0; (X).ep_mask = TBE_INTERVAL_EP_NUL
#define TBE_INTERVAL_INIT2(X,Y1,Y2,Y3) \
  (X).ep_1 = (Y1); \
  (X).ep_2 = (Y2); \
  (X).ep_mask = (Y3)

#endif
