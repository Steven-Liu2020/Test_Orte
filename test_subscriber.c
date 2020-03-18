/*
 *  $Id: h_subscriber.c,v 0.0.1.0       2005/01/03
 *
 *  DEBUG:  section                     h_subscriber
 *
 *  -------------------------------------------------------------------  
 *                                ORTE                                 
 *                      Open Real-Time Ethernet                       
 *                                                                    
 *                      Copyright (C) 2001-2006                       
 *  Department of Control Engineering FEE CTU Prague, Czech Republic  
 *                      http://dce.felk.cvut.cz                       
 *                      http://www.ocera.org                          
 *                                                                    
 *  Author: 		 Petr Smolik	petr@smoliku.cz             
 *  Advisor: 		 Pavel Pisa                                   
 *  Project Responsible: Zdenek Hanzalek                              
 *  --------------------------------------------------------------------
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 */
#include "orte.h"
#include <stdio.h>

ORTEDomain              *d;
static int              domain = 1;
static char             instance2Recv[64];
int                     regfail=0;
ORTEDomainAppEvents     events;
//event system
void
onRegFail(void *param) {
  printf("registration to a manager failed\n");
  regfail=1;
}


static void
recvCallBack(const ORTERecvInfo *info,void *vinstance, void *recvCallBackParam) {
  char *instance=(char*)vinstance;

  switch (info->status) {
    case NEW_DATA:
      printf("receive issue: %s\n",instance);
      break;
    case DEADLINE:
      printf("deadline occurred\n");
      break;
  }
}


static void *subscriberCreate(void *arg) {
  ORTESubscription    *s;
  NtpTime             deadline,minimumSeparation;

  ORTETypeRegisterAdd(d,"HelloMsg",NULL,NULL,NULL,sizeof(instance2Recv));
  NTPTIME_BUILD(deadline,5);
  NTPTIME_BUILD(minimumSeparation,0);
  s=ORTESubscriptionCreate(
       d,
       IMMEDIATE,
       BEST_EFFORTS,
       "Example HelloMsg",
       "HelloMsg",
       instance2Recv,
       &deadline,
       &minimumSeparation,
       recvCallBack,
       NULL,
       IPADDRESS_INVALID);
  if (s == NULL) {
    printf("ORTESubscriptionCreate failed\n");
  }
  return arg;
}


int main(int argc, char *args[]) {
  ORTEInit();
  //ORTEVerbositySetOptions("ALL.10");
  //initiate event system
  ORTEDomainInitEvents(&events);
  events.onRegFail=onRegFail;
  d=ORTEDomainAppCreate(domain,NULL,&events,ORTE_FALSE);
  if (!d) {
    printf("ORTEDomainAppCreate failed!\n");
    return 0;
  }
  subscriberCreate(NULL);
  while (!regfail)
    ORTESleepMs(1000);
  return 0;
}








