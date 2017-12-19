//
//  PMSPromiseBox.h
//  Promissory
//
//  Created by Kevin Ballard on 12/12/17.
//  Copyright © 2017 Kevin Ballard. All rights reserved.
//

#import <Foundation/Foundation.h>

typedef NS_ENUM(int, PMSPromiseBoxState) {
    PMSPromiseBoxStateEmpty,
    PMSPromiseBoxStateResolving,
    PMSPromiseBoxStateResolved,
    PMSPromiseBoxStateCancelling,
    PMSPromiseBoxStateCancelled
};

/// This is a sentinel value. DO NOT DEREFERENCE.
static void * const _Nonnull PMSLinkedListSwapFailed = (void *)0x1;

@interface PMSPromiseBox : NSObject
@property (atomic, readonly) PMSPromiseBoxState state;
/// Return the state without issuing a fence.
@property (atomic, readonly) PMSPromiseBoxState unfencedState;

- (nonnull instancetype)init NS_DESIGNATED_INITIALIZER;
- (nonnull instancetype)initWithState:(PMSPromiseBoxState)state NS_DESIGNATED_INITIALIZER;

/// Returns \c YES if the state is transitioned successfully or \c NO if it is not.
///
/// Valid state transitions:
///
/// \li Empty -> Resolving
/// \li Empty -> Cancelling
/// \li Empty -> Cancelled
/// \li Resolving -> Resolved
/// \li Cancelling -> Resolving
/// \li Cancelling -> Cancelled
///
/// Transitioning to the current state is considered a failure and \c NO is returned.
- (BOOL)transitionStateTo:(PMSPromiseBoxState)state __attribute__((warn_unused_result));

/// Atomically swaps the callback linked list pointer.
///
/// If the existing linked list pointer is \c PMSLinkedListSwapFailed no swap is performed.
///
/// \param node The node to push onto the head of the list.
/// \param linkBlock A block that is invoked with the previous head prior to pushing the new node
/// on. This block should modify the new node to link to the previous head. If multiple threads are
/// swapping the list at the same time, this block may be invoked multiple times.
/// \returns The old value of the linked list, or \c PMSLinkedListSwapFailed if the swap failed.
- (nullable void *)swapCallbackLinkedListWith:(nullable void *)node linkBlock:(nullable void (^)(void * _Nullable nextNode))linkBlock __attribute__((warn_unused_result));
/// Atomically swaps the request cancel linked list pointer.
///
/// If the existing linked list pointer is \c PMSLinkedListSwapFailed no swap is performed.
///
/// \param node The node to push onto the head of the list.
/// \param linkBlock A block that is invoked with the previous head prior to pushing the new node
/// on. This block should modify the new node to link to the previous head. If multiple threads are
/// swapping the list at the same time, this block may be invoked multiple times.
/// \returns The old value of the linked list, or \c PMSLinkedListSwapFailed if the swap failed.
- (nullable void *)swapRequestCancelLinkedListWith:(nullable void *)node linkBlock:(nullable void (^)(void * _Nullable nextNode))linkBlock __attribute__((warn_unused_result));

- (void)issueDeinitFence;
@end