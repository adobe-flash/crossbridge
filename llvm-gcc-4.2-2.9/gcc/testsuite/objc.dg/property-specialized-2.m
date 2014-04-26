/* APPLE LOCAL file radar 5218071 */
/* Test that property type in the derived class can be more specialized than
   that of its base class. Issue warning when this is not the case.
*/
/* { dg-options "-mmacosx-version-min=10.5" } */
/* { dg-do compile } */

@protocol P1 @end
@protocol P2 @end
@protocol P3 @end

@interface NSData @end

@interface MutableNSData : NSData @end

@interface Base : NSData <P1>
@property(readonly) id ref;
@property(readonly) Base *p_base;
@property(readonly) NSData *nsdata;
@property(readonly) NSData * m_nsdata;
@end

@interface Data : Base <P1, P2>
@property(readonly) NSData *ref;
@property(readonly) Data *p_base;	// warn
@property(readonly) MutableNSData * m_nsdata;
@end /* { dg-warning "property 'p_base' type does not match super class 'Base' property type" } */

@interface  MutedData: Data
@property(readonly) id p_base;
@end	/* { dg-warning "property 'p_base' type does not match super class 'Data' property type" } */

@interface ConstData : Data <P1, P2, P3>
@property(readonly) ConstData *p_base;
@end /* { dg-warning "property 'p_base' type does not match super class 'Data' property type" } */

