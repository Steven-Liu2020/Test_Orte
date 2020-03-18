/*
 *  $Id: h_publisher.c,v 0.0.1.0        2005/01/03
 *
 *  DEBUG:  section                     h_publisher
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
static char              instance2Send[64];
static int               counter=0;
int                      regfail=0;
ORTEDomainAppEvents      events;
//event system
void
onRegFail(void *param) {
  printf("registration to a manager failed\n");
  regfail=1;
}

static void
sendCallBack(const ORTESendInfo *info,void *vinstance, void *sendCallBackParam) {
  char *instance=(char*)vinstance;
  char *num;
  switch (info->status) {
    case NEED_DATA:
      sprintf(instance,"Hello World,number: %d",counter++);
      printf("Sampling publication,  %s\n", instance);
      break;
    case CQL:  //criticalQueueLevel
      break;
  }
}

void * publisherCreate(void *arg) {
  ORTEPublication     *p;
  NtpTime             persistence, delay;

  ORTETypeRegisterAdd(d,"HelloMsg",NULL,NULL,NULL,sizeof(instance2Send));
  NTPTIME_BUILD(persistence,3);
  NTPTIME_BUILD(delay,1); 
  p=ORTEPublicationCreate(
       d,
      "Example HelloMsg",
      "HelloMsg",
      instance2Send,
      &persistence,
      1,
      sendCallBack,
      NULL,
      &delay);
  if (p == NULL) {
    printf("ORTEPublicationCreate failed\n");
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
  publisherCreate((void*)d);
  while(!regfail) 
    ORTESleepMs(1000);

  ORTESleepMs(10000);
  printf("finnished!\n");
  ORTEDomainAppDestroy(d);
  return 0;
}


