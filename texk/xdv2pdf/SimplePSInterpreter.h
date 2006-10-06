//
//  SimplePSInterpreter.h
//  T1Wrap
//
//  Created by Massimiliano Gubinelli on 10/02/2005.
//  Copyright 2005 Massimilano Gubinelli. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface SimplePSInterpreter : NSObject {
  NSMutableArray *stack, *dicts;
  NSMutableDictionary *fontDir;
  NSMutableString *inputStream;
  NSScanner *sc;
  int nStoringProc;
}
- (void)doString:(NSString *)anInputString;
- (NSDictionary*)fontDir;
@end
