#pragma once
#include "packets.hpp"
#include "tcb.hpp"

namespace mstack {
class tcp_transmit {
public:
        static void tcp_in(std::shared_ptr<tcb_t> in_tcb, tcp_packet_t& in_packet) {}

        // void tcp_in(tcp_packet& in_packet) {
        //         full_connect_id_t full_connect_id(in_packet._remote_info.value(),
        //                                           in_packet._local_info.value());
        //         std::optional<std::shared_ptr<full_tcb_t>> full_tcb =
        //                 tcp_manager.query_and_register_full_tcb(full_connect_id);
        //         if (full_tcb) {
        //                 tcp_state_transition(in_packet, full_tcb.value());
        //                 return;
        //         }
        //         DLOG(ERROR) << "[ERROR FULL CONNECT]";
        // }

        // void tcp_send_ack() {}
        // void tcp_send_syn_ack() {}
        // void tcp_send_rst() {}
        // void tcp_send_ctl(l4_packet&                  in_packet,
        //                   std::shared_ptr<full_tcb_t> in_tcb,
        //                   int                         ctl,
        //                   uint32_t                    seq_no,
        //                   uint32_t                    ack_no) {
        //         tcp_header_t tcp_header;
        //         tcp_header.src_port              = in_packet._local_info->port_addr;
        //         tcp_header.dst_port              = in_packet._remote_info->port_addr;
        //         tcp_header.ack_no                = ack_no;
        //         tcp_header.seq_no                = seq_no;
        //         tcp_header.NOP                   = 0x0;
        //         tcp_header.URG                   = ((ctl & TCP_URG) != 0);
        //         tcp_header.ACK                   = ((ctl & TCP_ACK) != 0);
        //         tcp_header.PSH                   = ((ctl & TCP_PSH) != 0);
        //         tcp_header.RST                   = ((ctl & TCP_RST) != 0);
        //         tcp_header.SYN                   = ((ctl & TCP_SYN) != 0);
        //         tcp_header.FIN                   = ((ctl & TCP_FIN) != 0);
        //         tcp_header.checksum              = 0x0000;
        //         tcp_header.window_size           = 0xFAF0;
        //         tcp_header.urgent_pointer        = 0x0000;
        //         tcp_header.header_length         = tcp_header_t::size() / 4;
        //         std::unique_ptr<packet> out_data = std::make_unique<packet>(tcp_header_t::size());
        //         tcp_header.produce(out_data->get_pointer());

        //         l4_packet out_packet(in_packet._remote_info, in_packet._local_info, TCP_PROTO,
        //                              std::move(out_data));
        //         in_tcb->enqueue_packet(std::move(out_packet));
        //         active_tcbs.push_back(in_tcb);
        // }

        // int generate_iss() { return 0; }

        // bool tcp_handle_close_state(l4_packet& in_packet, std::shared_ptr<full_tcb_t> in_tcb) {
        //         tcp_header_t in_tcp;
        //         in_tcp.consume(in_packet._payload->get_pointer());
        //         /**
        //          *  If the state is CLOSED (i.e., TCB does not exist) then
        //          *  all data in the incoming segment is discarded.  An incoming
        //          *  segment containing a RST is discarded.
        //          */
        //         if (in_tcp.RST == 1) {
        //                 return true;
        //         }
        //         /**
        //          *  An incoming segment not
        //          *  containing a RST causes a RST to be sent in response.  The
        //          *  acknowledgment and sequence field values are selected to make the
        //          *  reset sequence acceptable to the TCP that sent the offending
        //          *  segment.
        //          *
        //          *  If the ACK bit is off, sequence number zero is used,
        //          *      <SEQ=0><ACK=SEG.SEQ+SEG.LEN><CTL=RST,ACK>
        //          */
        //         if (in_tcp.ACK == 0) {
        //                 return true;
        //         }

        //         /**
        //          *  If the ACK bit is on,
        //          *       <SEQ=SEG.ACK><CTL=RST>. Return.
        //          */

        //         if (in_tcp.ACK == 1) {
        //                 return true;
        //         }
        // }

        // bool tcp_handle_listen_state(l4_packet& in_packet, std::shared_ptr<full_tcb_t> in_tcb) {
        //         //  If the state is LISTEN then
        //         if (in_tcb->state != TCP_LISTEN) {
        //                 return false;
        //         }

        //         tcp_header_t in_tcp;
        //         in_tcp.consume(in_packet._payload->get_pointer());

        //         /**
        //          *  first check for an RST
        //          *  An incoming RST should be ignored.  Return.
        //          */
        //         if (in_tcp.RST == 1) {
        //                 return true;
        //         }

        //         /**
        //          *  TODO: second check for an ACK
        //          *  Any acknowledgment is bad if it arrives on a connection still in
        //          *  the LISTEN state.  An acceptable reset segment should be formed
        //          *  for any arriving ACK-bearing segment.  The RST should be
        //          *  formatted as follows: <SEQ=SEG.ACK><CTL=RST>
        //          */
        //         if (in_tcp.ACK == 1) {
        //                 tcp_send_rst();
        //                 return true;
        //         }

        //         /**
        //          *  TODO: third check for a SYN
        //          *  If the SYN bit is set, check the security.  If the
        //          *  security/compartment on the incoming segment does not exactly
        //          *  match the security/compartment in the TCB then send a reset and
        //          *  return. <SEQ=SEG.ACK><CTL=RST>
        //          *
        //          *  If the SEG.PRC is greater than the TCB.PRC then if allowed by
        //          *  the user and the system set TCB.PRC<-SEG.PRC, if not allowed
        //          *  send a reset and return. <SEQ=SEG.ACK><CTL=RST>
        //          *
        //          *  If the SEG.PRC is less than the TCB.PRC then continue.
        //          */
        //         if (in_tcp.SYN == 1) {
        //         }
        //         /**
        //          *  Set RCV.NXT to SEG.SEQ+1, IRS is set to SEG.SEQ and any other
        //          *  control or text should be queued for processing later.  ISS
        //          *  should be selected and a SYN segment sent of the form:
        //          *  <SEQ=ISS><ACK=RCV.NXT><CTL=SYN,ACK>
        //          *
        //          *  SND.NXT is set to ISS+1 and SND.UNA to ISS.  The connection
        //          *  state should be changed to SYN-RECEIVED.  Note that any other
        //          *  incoming control or data (combined with SYN) will be processed
        //          *  in the SYN-RECEIVED state, but processing of SYN and ACK should
        //          *  not be repeated.  If the listen was not fully specified (i.e.,
        //          *  the foreign socket was not fully specified), then the
        //          *  unspecified fields should be filled in now.
        //          */

        //         if (in_tcp.SYN == 1) {
        //                 uint32_t iss            = generate_iss();
        //                 in_tcb->receiver.next   = in_tcp.seq_no + 1;
        //                 in_tcb->receiver.window = 0xFAF0;
        //                 tcp_send_ctl(in_packet, in_tcb, TCP_SYN | TCP_ACK, iss,
        //                              in_tcb->receiver.next);
        //                 in_tcb->sender.next           = iss + 1;
        //                 in_tcb->sender.unacknowledged = iss;
        //                 in_tcb->state                 = TCP_SYN_RECEIVED;
        //                 return false;
        //         }

        //         /**
        //          *  TODO: fourth other text or control
        //          *  must have an ACK and thus would be discarded by the ACK
        //          *  processing.  An incoming RST segment could not be valid, since
        //          *  it could not have been sent in response to anything sent by this
        //          *  incarnation of the connection.  So you are unlikely to get here,
        //          *  but if you do, drop the segment, and return.
        //          */
        //         return true;
        // }

        // bool tcp_handle_syn_sent(l4_packet& in_packet, std::shared_ptr<full_tcb_t> in_tcb) {
        //         // If the state is SYN-SENT then
        //         if (in_tcb->state != TCP_SYN_SENT) {
        //                 return false;
        //         }

        //         tcp_header_t in_tcp;
        //         in_tcp.consume(in_packet._payload->get_pointer());
        //         // first check the ACK bit
        //         if (in_tcp.ACK == 1) {
        //                 /**
        //                  *  If SEG.ACK =< ISS, or SEG.ACK > SND.NXT, send a reset (unless
        //                  *  the RST bit is set, if so drop the segment and return)
        //                  *      <SEQ=SEG.ACK><CTL=RST>
        //                  *  and discard the segment.  Return.
        //                  *  If SND.UNA =< SEG.ACK =< SND.NXT then the ACK is acceptable.
        //                  */
        //         }

        //         // second check the RST bit
        //         if (in_tcp.RST == 1) {
        //                 /**
        //                  *  If the ACK was acceptable then signal the user "error:
        //                  *  connection reset", drop the segment, enter CLOSED state,
        //                  *  delete TCB, and return.  Otherwise (no ACK) drop the segment
        //                  *  and return.
        //                  */
        //         }

        //         // third check the security and precedence
        //         /**
        //          *  If the security/compartment in the segment does not exactly
        //          *  match the security/compartment in the TCB, send a reset
        //          *
        //          *  If there is an ACK
        //          *
        //          *      <SEQ=SEG.ACK><CTL=RST>
        //          *
        //          *  Otherwise
        //          *
        //          *      <SEQ=0><ACK=SEG.SEQ+SEG.LEN><CTL=RST,ACK>
        //          *
        //          *  If there is an ACK
        //          *
        //          *  The precedence in the segment must match the precedence in the
        //          *  TCB, if not, send a reset
        //          *
        //          *      <SEQ=SEG.ACK><CTL=RST>
        //          *
        //          *  If there is no ACK
        //          *
        //          *  If the precedence in the segment is higher than the precedence
        //          *  in the TCB then if allowed by the user and the system raise
        //          *  the precedence in the TCB to that in the segment, if not
        //          *  allowed to raise the prec then send a reset.
        //          *
        //          *      <SEQ=0><ACK=SEG.SEQ+SEG.LEN><CTL=RST,ACK>
        //          *
        //          *  If the precedence in the segment is lower than the precedence
        //          *  in the TCB continue.
        //          *
        //          *  If a reset was sent, discard the segment and return.
        //          */

        //         // fourth check the SYN bit

        //         /**
        //          *   This step should be reached only if the ACK is ok, or there is
        //          *   no ACK, and it the segment did not contain a RST.
        //          *
        //          *   If the SYN bit is on and the security/compartment and precedence
        //          *   are acceptable then, RCV.NXT is set to SEG.SEQ+1, IRS is set to
        //          *   SEG.SEQ.  SND.UNA should be advanced to equal SEG.ACK (if there
        //          *   is an ACK), and any segments on the retransmission queue which
        //          *   are thereby acknowledged should be removed.
        //          *
        //          *   If SND.UNA > ISS (our SYN has been ACKed), change the connection
        //          *   state to ESTABLISHED, form an ACK segment
        //          *
        //          *      <SEQ=SND.NXT><ACK=RCV.NXT><CTL=ACK>
        //          *
        //          *   and send it.  Data or controls which were queued for
        //          *   transmission may be included.  If there are other controls or
        //          *   text in the segment then continue processing at the sixth step
        //          *   below where the URG bit is checked, otherwise return.
        //          *
        //          *   Otherwise enter SYN-RECEIVED, form a SYN,ACK segment
        //          *
        //          *      <SEQ=ISS><ACK=RCV.NXT><CTL=SYN,ACK>
        //          *
        //          *   and send it.  If there are other controls or text in the
        //          *   segment, queue them for processing after the ESTABLISHED state
        //          *   has been reached, return.
        //          */

        //         // fifth, if neither of the SYN or RST bits is set then drop the segment
        //         // and return.
        //         return true;
        // }

        // bool tcp_check_segment(l4_packet& in_packet, std::shared_ptr<full_tcb_t> in_tcb) {
        //         /**
        //          *  SYN-RECEIVED STATE
        //          *  ESTABLISHED STATE
        //          *  FIN-WAIT-1 STATE
        //          *  FIN-WAIT-2 STATE
        //          *  CLOSE-WAIT STATE
        //          *  CLOSING STATE
        //          *  LAST-ACK STATE
        //          *  TIME-WAIT STATE
        //          *
        //          *      Segments are processed in sequence.  Initial tests on arrival
        //          *      are used to discard old duplicates, but further processing is
        //          *      done in SEG.SEQ order.  If a segment's contents straddle the
        //          *      boundary between old and new, only the new parts should be
        //          *      processed.
        //          *
        //          *      There are four cases for the acceptability test for an incoming
        //          *      segment:
        //          *
        //          *      Segment Receive  Test
        //          *      Length  Window
        //          *      ------- -------  -------------------------------------------
        //          *
        //          *      0       0     SEG.SEQ = RCV.NXT
        //          *
        //          *      0      >0     RCV.NXT =< SEG.SEQ < RCV.NXT+RCV.WND
        //          *
        //          *      >0       0     not acceptable
        //          *
        //          *      >0      >0     RCV.NXT =< SEG.SEQ < RCV.NXT+RCV.WND
        //          *                  or RCV.NXT =< SEG.SEQ+SEG.LEN-1 < RCV.NXT+RCV.WND
        //          *
        //          *      If the RCV.WND is zero, no segments will be acceptable, but
        //          *      special allowance should be made to accept valid ACKs, URGs and
        //          *      RSTs.
        //          *
        //          *      If an incoming segment is not acceptable, an acknowledgment
        //          *      should be sent in reply (unless the RST bit is set, if so drop
        //          *      the segment and return):
        //          *
        //          *      <SEQ=SND.NXT><ACK=RCV.NXT><CTL=ACK>
        //          *
        //          *      After sending the acknowledgment, drop the unacceptable segment
        //          *      and return.
        //          *      In the following it is assumed that the segment is the idealized
        //          *      segment that begins at RCV.NXT and does not exceed the window.
        //          *      One could tailor actual segments to fit this assumption by
        //          *      trimming off any portions that lie outside the window (including
        //          *      SYN and FIN), and only processing further if the segment then
        //          *      begins at RCV.NXT.  Segments with higher begining sequence
        //          *      numbers may be held for later processing.
        //          */

        //         tcp_header_t in_tcp;
        //         in_tcp.consume(in_packet._payload->get_pointer());
        //         uint16_t segment_length =
        //                 in_packet._payload->get_remaining_len() - in_tcp.header_length * 2;

        //         switch (in_tcb->state) {
        //                 case TCP_SYN_RECEIVED:
        //                 case TCP_ESTABLISHED:
        //                 case TCP_FIN_WAIT_1:
        //                 case TCP_FIN_WAIT_2:
        //                 case TCP_CLOSE_WAIT:
        //                 case TCP_CLOSING:
        //                 case TCP_LAST_ACK:
        //                 case TCP_TIME_WAIT:
        //                         if (segment_length == 0 && in_tcb->receiver.window == 0) {
        //                                 if (in_tcp.seq_no == in_tcb->receiver.next) {
        //                                         return true;
        //                                 } else {
        //                                         return false;
        //                                 }
        //                         }
        //                         if (segment_length == 0 && in_tcb->receiver.window > 0) {
        //                                 // RCV.NXT =< SEG.SEQ < RCV.NXT+RCV.WND
        //                                 if (in_tcb->receiver.next <= in_tcp.seq_no &&
        //                                     in_tcp.seq_no < in_tcb->receiver.next +
        //                                                             in_tcb->receiver.window) {
        //                                         return true;
        //                                 } else {
        //                                         return false;
        //                                 }
        //                         }
        //                         if (segment_length > 0 && in_tcb->receiver.window == 0) {
        //                                 return false;
        //                         }
        //                         if (segment_length > 0 && in_tcb->receiver.window > 0) {
        //                                 // RCV.NXT =< SEG.SEQ < RCV.NXT+RCV.WND
        //                                 //        or RCV.NXT =< SEG.SEQ+SEG.LEN-1 < RCV.NXT+RCV.WND
        //                                 if ((in_tcb->receiver.next <= in_tcp.seq_no &&
        //                                      in_tcb->receiver.next + in_tcb->receiver.window) ||
        //                                     (in_tcb->receiver.next <=
        //                                              in_tcp.seq_no + segment_length - 1 &&
        //                                      in_tcp.seq_no + segment_length - 1 <
        //                                              in_tcb->receiver.next +
        //                                                      in_tcb->receiver.window)) {
        //                                         return true;
        //                                 } else {
        //                                         return false;
        //                                 }
        //                         }
        //         }
        // }

        // void tcp_state_transition(l4_packet& in_packet, std::shared_ptr<full_tcb_t> in_tcb) {
        //         DLOG(INFO) << "[TCP] [CHECK TCP_CLOSED] " << *in_tcb;
        //         if (in_tcb->state == TCP_CLOSED && tcp_handle_close_state(in_packet, in_tcb)) {
        //                 return;
        //         }

        //         DLOG(INFO) << "[TCP] [CHECK TCP_LISTEN] " << *in_tcb;
        //         if (in_tcb->state == TCP_LISTEN && tcp_handle_listen_state(in_packet, in_tcb)) {
        //                 return;
        //         }

        //         DLOG(INFO) << "[TCP] [CHECK TCP_SYN_SENY] " << *in_tcb;
        //         if (in_tcb->state == TCP_SYN_SENT && tcp_handle_syn_sent(in_packet, in_tcb)) {
        //                 return;
        //         }

        //         tcp_header_t in_tcp;
        //         in_tcp.consume(in_packet._payload->get_pointer());

        //         DLOG(INFO) << "[TCP] [PROCESS 1] " << *in_tcb;
        //         // first check sequence number
        //         if (!tcp_check_segment(in_packet, in_tcb)) {
        //                 if (!in_tcp.RST) {
        //                         // <SEQ=SND.NXT><ACK=RCV.NXT><CTL=ACK>
        //                         tcp_send_ack();
        //                 }
        //                 return;
        //         }

        //         DLOG(INFO) << "[TCP] [PROCESS 2] " << *in_tcb;
        //         // TODO: second check the RST bit
        //         if (in_tcp.RST == 1) {
        //                 switch (in_tcb->state) {
        //                         /**
        //                          *  SYN-RECEIVED STATE
        //                          *      If the RST bit is set
        //                          *      If this connection was initiated with a passive OPEN (i.e.,
        //                          *      came from the LISTEN state), then return this connection to
        //                          *      LISTEN state and return.  The user need not be informed.  If
        //                          *      this connection was initiated with an active OPEN (i.e.,
        //                          * came from SYN-SENT state) then the connection was refused, signal
        //                          *      the user "connection refused".  In either case, all segments
        //                          *      on the retransmission queue should be removed.  And in the
        //                          *      active OPEN case, enter the CLOSED state and delete the TCB,
        //                          *      and return.
        //                          */
        //                         case TCP_SYN_RECEIVED:
        //                                 return;
        //                         /**
        //                          *  ESTABLISHED
        //                          *  FIN-WAIT-1
        //                          *  FIN-WAIT-2
        //                          *  CLOSE-WAIT
        //                          *      If the RST bit is set then, any outstanding RECEIVEs and
        //                          * SEND should receive "reset" responses.  All segment queues should
        //                          * be flushed.  Users should also receive an unsolicited general
        //                          *      "connection reset" signal.  Enter the CLOSED state, delete
        //                          * the TCB, and return.
        //                          */
        //                         case TCP_ESTABLISHED:
        //                         case TCP_FIN_WAIT_1:
        //                         case TCP_FIN_WAIT_2:
        //                         case TCP_CLOSE_WAIT:
        //                                 return;
        //                         /**
        //                          *  CLOSING STATE
        //                          *  LAST-ACK STATE
        //                          *  TIME-WAIT
        //                          *      If the RST bit is set then, enter the CLOSED state, delete
        //                          * the TCB, and return.
        //                          */
        //                         case TCP_CLOSING:
        //                         case TCP_LAST_ACK:
        //                         case TCP_TIME_WAIT:
        //                                 return;
        //                 }
        //         }
        //         DLOG(INFO) << "[TCP] [PROCESS 3] " << *in_tcb;
        //         // TODO: third check security and precedence
        //         /**
        //          *  SYN-RECEIVED
        //          *
        //          *      If the security/compartment and precedence in the segment do not
        //          *      exactly match the security/compartment and precedence in the TCB
        //          *      then send a reset, and return.
        //          *
        //          *  ESTABLISHED STATE
        //          *
        //          *      If the security/compartment and precedence in the segment do not
        //          *      exactly match the security/compartment and precedence in the TCB
        //          *      then send a reset, any outstanding RECEIVEs and SEND should
        //          *      receive "reset" responses.  All segment queues should be
        //          *      flushed.  Users should also receive an unsolicited general
        //          *      "connection reset" signal.  Enter the CLOSED state, delete the
        //          *      TCB, and return.
        //          *
        //          *  Note this check is placed following the sequence check to prevent
        //          *  a segment from an old connection between these ports with a
        //          *  different security or precedence from causing an abort of the
        //          *  current connection.
        //          */
        //         DLOG(INFO) << "[TCP] [PROCESS 4] " << *in_tcb;
        //         // TODO: fourth, check the SYN bit
        //         if (in_tcp.SYN) {
        //                 switch (in_tcb->state) {
        //                                 /**
        //                                  *  SYN-RECEIVED
        //                                  *  ESTABLISHED STATE
        //                                  *  FIN-WAIT STATE-1
        //                                  *  FIN-WAIT STATE-2
        //                                  *  CLOSE-WAIT STATE
        //                                  *  CLOSING STATE
        //                                  *  LAST-ACK STATE
        //                                  *  TIME-WAIT STATE
        //                                  *
        //                                  *      If the SYN is in the window it is an error, send a
        //                                  * reset, any outstanding RECEIVEs and SEND should receive
        //                                  * "reset" responses, all segment queues should be flushed,
        //                                  * the user should also receive an unsolicited general
        //                                  * "connection reset" signal, enter the CLOSED state, delete
        //                                  * the TCB, and return.
        //                                  *
        //                                  *      If the SYN is not in the window this step would not
        //                                  * be reached and an ack would have been sent in the first
        //                                  * step (sequence number check).
        //                                  */
        //                         case TCP_SYN_RECEIVED:
        //                         case TCP_ESTABLISHED:
        //                         case TCP_FIN_WAIT_1:
        //                         case TCP_FIN_WAIT_2:
        //                         case TCP_CLOSE_WAIT:
        //                         case TCP_CLOSING:
        //                         case TCP_LAST_ACK:
        //                         case TCP_TIME_WAIT:
        //                                 return;
        //                 }
        //         }
        //         // fifth check the ACK field
        //         DLOG(INFO) << "[TCP] [PROCESS 5] " << *in_tcb;
        //         if (in_tcp.ACK) {
        //                 switch (in_tcb->state) {
        //                         /**
        //                          *  SYN-RECEIVED STATE
        //                          *      If SND.UNA =< SEG.ACK =< SND.NXT then enter ESTABLISHED
        //                          * state and continue processing. If the segment acknowledgment is
        //                          * not acceptable, form a reset segment, <SEQ=SEG.ACK><CTL=RST>
        //                          */
        //                         case TCP_SYN_RECEIVED:
        //                                 if (in_tcb->sender.unacknowledged <= in_tcp.ack_no &&
        //                                     in_tcp.ack_no <= in_tcb->sender.next) {
        //                                         in_tcb->state = TCP_ESTABLISHED;
        //                                 } else {
        //                                         tcp_send_rst();
        //                                         return;
        //                                 }
        //                                 break;
        //                         /**
        //                          *  ESTABLISHED STATE
        //                          *      If SND.UNA < SEG.ACK =< SND.NXT then, set SND.UNA <-
        //                          * SEG.ACK. Any segments on the retransmission queue which are
        //                          * thereby entirely acknowledged are removed.  Users should receive
        //                          *      positive acknowledgments for buffers which have been SENT
        //                          * and fully acknowledged (i.e., SEND buffer should be returned with
        //                          *      "ok" response).  If the ACK is a duplicate
        //                          *      (SEG.ACK < SND.UNA), it can be ignored.  If the ACK acks
        //                          *      something not yet sent (SEG.ACK > SND.NXT) then send an ACK,
        //                          *      drop the segment, and return.
        //                          *
        //                          *      If SND.UNA < SEG.ACK =< SND.NXT, the send window should be
        //                          *      updated.  If (SND.WL1 < SEG.SEQ or (SND.WL1 = SEG.SEQ and
        //                          *      SND.WL2 =< SEG.ACK)), set SND.WND <- SEG.WND, set
        //                          *      SND.WL1 <- SEG.SEQ, and set SND.WL2 <- SEG.ACK.
        //                          *
        //                          *      Note that SND.WND is an offset from SND.UNA, that SND.WL1
        //                          *      records the sequence number of the last segment used to
        //                          * update SND.WND, and that SND.WL2 records the acknowledgment
        //                          * number of the last segment used to update SND.WND.  The check
        //                          * here prevents using old segments to update the window.
        //                          */
        //                         case TCP_ESTABLISHED:
        //                         case TCP_FIN_WAIT_1:
        //                         case TCP_FIN_WAIT_2:
        //                         case TCP_CLOSE_WAIT:
        //                         case TCP_CLOSING:
        //                                 if (in_tcb->sender.unacknowledged < in_tcp.ack_no &&
        //                                     in_tcp.ack_no <= in_tcb->sender.next) {
        //                                         in_tcb->sender.unacknowledged = in_tcp.ack_no;
        //                                         // TODO: update windows
        //                                 }
        //                                 if (in_tcp.ack_no <
        //                                     in_tcb->sender.unacknowledged) { /* ignore */
        //                                 }
        //                                 if (in_tcp.ack_no > in_tcb->sender.next) {
        //                                         tcp_send_ack();
        //                                         return;
        //                                 }

        //                                 /**
        //                                  *  FIN-WAIT-1 STATE
        //                                  *      In addition to the processing for the ESTABLISHED
        //                                  * state, if our FIN is now acknowledged then enter
        //                                  * FIN-WAIT-2 and continue processing in that state.
        //                                  */
        //                                 if (in_tcb->state == TCP_FIN_WAIT_1) {
        //                                         in_tcb->state = TCP_FIN_WAIT_2;
        //                                 }

        //                                 /**
        //                                  *  FIN-WAIT-2 STATE
        //                                  *      In addition to the processing for the ESTABLISHED
        //                                  * state, if the retransmission queue is empty, the user's
        //                                  * CLOSE can be acknowledged ("ok") but do not delete the
        //                                  * TCB.
        //                                  */
        //                                 if (in_tcb->state == TCP_FIN_WAIT_2) {
        //                                 }
        //                                 /**
        //                                  *  CLOSE-WAIT STATE
        //                                  *      Do the same processing as for the ESTABLISHED state.
        //                                  */

        //                                 if (in_tcb->state == TCP_CLOSE_WAIT) {
        //                                 }
        //                                 /**
        //                                  *  CLOSING STATE
        //                                  *      In addition to the processing for the ESTABLISHED
        //                                  * state, if the ACK acknowledges our FIN then enter the
        //                                  * TIME-WAIT state, otherwise ignore the segment.
        //                                  */
        //                                 if (in_tcb->state == TCP_CLOSING) {
        //                                 }
        //                                 break;
        //                         /**
        //                          *  LAST-ACK STATE
        //                          *      The only thing that can arrive in this state is an
        //                          *      acknowledgment of our FIN.  If our FIN is now acknowledged,
        //                          *      delete the TCB, enter the CLOSED state, and return.
        //                          */
        //                         case TCP_LAST_ACK:
        //                                 in_tcb->state == TCP_CLOSED;
        //                                 return;
        //                         /**
        //                          *  TIME-WAIT STATE
        //                          *      The only thing that can arrive in this state is a
        //                          *      retransmission of the remote FIN.  Acknowledge it, and
        //                          * restart the 2 MSL timeout.
        //                          */
        //                         case TCP_TIME_WAIT:
        //                                 tcp_send_ack();
        //                                 return;
        //                 }
        //         }

        //         DLOG(INFO) << "[TCP] [PROCESS 6] " << *in_tcb;
        //         // TODO: sixth, check the URG bit
        //         if (in_tcp.URG == 1) {
        //                 switch (in_tcb->state) {
        //                         /**
        //                          *  ESTABLISHED STATE
        //                          *  FIN-WAIT-1 STATE
        //                          *  FIN-WAIT-2 STATE
        //                          *      If the URG bit is set, RCV.UP <- max(RCV.UP,SEG.UP), and
        //                          * signal the user that the remote side has urgent data if the
        //                          * urgent pointer (RCV.UP) is in advance of the data consumed.  If
        //                          * the user has already been signaled (or is still in the "urgent
        //                          *      mode") for this continuous sequence of urgent data, do not
        //                          *      signal the user again.
        //                          */
        //                         case TCP_ESTABLISHED:
        //                         case TCP_FIN_WAIT_1:
        //                         case TCP_FIN_WAIT_2:

        //                         /**
        //                          *  CLOSE-WAIT STATE
        //                          *  CLOSING STATE
        //                          *  LAST-ACK STATE
        //                          *  TIME-WAIT
        //                          *      This should not occur, since a FIN has been received from
        //                          * the remote side.  Ignore the URG.
        //                          */
        //                         case TCP_CLOSE_WAIT:
        //                         case TCP_CLOSING:
        //                         case TCP_LAST_ACK:
        //                         case TCP_TIME_WAIT:
        //                                 return;
        //                 }
        //         }

        //         DLOG(INFO) << "[TCP] [PROCESS 7] " << *in_tcb;
        //         // seventh, process the segment text
        //         switch (in_tcb->state) {
        //                         /**
        //                          *   ESTABLISHED STATE
        //                          *   FIN-WAIT-1 STATE
        //                          *   FIN-WAIT-2 STATE
        //                          *
        //                          *       Once in the ESTABLISHED state, it is possible to deliver
        //                          *       segment text to user RECEIVE buffers.  Text from segments
        //                          * can be moved into buffers until either the buffer is full or the
        //                          * segment is empty.  If the segment empties and carries an PUSH
        //                          * flag, then the user is informed, when the buffer is returned,
        //                          * that a PUSH has been received.
        //                          *
        //                          *       When the TCP takes responsibility for delivering the data
        //                          * to the user it must also acknowledge the receipt of the data.
        //                          *
        //                          *       Once the TCP takes responsibility for the data it advances
        //                          *       RCV.NXT over the data accepted, and adjusts RCV.WND as
        //                          *       apporopriate to the current buffer availability.  The total
        //                          * of RCV.NXT and RCV.WND should not be reduced.
        //                          *
        //                          *       Please note the window management suggestions in
        //                          * section 3.7.
        //                          *
        //                          *       Send an acknowledgment of the form:
        //                          *
        //                          *       <SEQ=SND.NXT><ACK=RCV.NXT><CTL=ACK>
        //                          *
        //                          *       This acknowledgment should be piggybacked on a segment
        //                          * being transmitted if possible without incurring undue delay
        //                          */
        //                 case TCP_ESTABLISHED:
        //                 case TCP_FIN_WAIT_1:
        //                 case TCP_FIN_WAIT_2:
        //                         /**
        //                          *  CLOSE-WAIT STATE
        //                          *  CLOSING STATE
        //                          *  LAST-ACK STATE
        //                          *  TIME-WAIT STATE
        //                          *      This should not occur, since a FIN has been received from
        //                          * the remote side.  Ignore the segment text.
        //                          */
        //                 case TCP_CLOSE_WAIT:
        //                 case TCP_CLOSING:
        //                 case TCP_LAST_ACK:
        //                 case TCP_TIME_WAIT:
        //                         return;
        //         }

        //         DLOG(INFO) << "[TCP] [PROCESS 8] " << *in_tcb;
        //         // eighth, check the FIN bit
        //         if (in_tcp.FIN == 1) {
        //                 switch (in_tcb->state) {
        //                                 /**
        //                                  *  Do not process the FIN if the state is CLOSED, LISTEN or
        //                                  * SYN-SENT since the SEG.SEQ cannot be validated; drop the
        //                                  * segment and return.
        //                                  *
        //                                  *  If the FIN bit is set, signal the user "connection
        //                                  * closing" and return any pending RECEIVEs with same
        //                                  * message, advance RCV.NXT over the FIN, and send an
        //                                  * acknowledgment for the FIN.  Note that FIN implies PUSH
        //                                  * for any segment text not yet delivered to the user.
        //                                  */

        //                                 /**
        //                                  *  SYN-RECEIVED STATE
        //                                  *  ESTABLISHED STATE
        //                                  *      Enter the CLOSE-WAIT state.
        //                                  */
        //                         case TCP_SYN_RECEIVED:
        //                         case TCP_ESTABLISHED:
        //                                 /**
        //                                  *  FIN-WAIT-1 STATE
        //                                  *      If our FIN has been ACKed (perhaps in this segment),
        //                                  * then enter TIME-WAIT, start the time-wait timer, turn off
        //                                  * the other timers; otherwise enter the CLOSING state.
        //                                  */
        //                         case TCP_FIN_WAIT_1:
        //                                 /**
        //                                  *  FIN-WAIT-2 STATE
        //                                  *      Enter the TIME-WAIT state.  Start the time-wait
        //                                  * timer, turn off the other timers.
        //                                  */
        //                         case TCP_FIN_WAIT_2:
        //                                 /**
        //                                  *  CLOSE-WAIT STATE
        //                                  *      Remain in the CLOSE-WAIT state.
        //                                  */
        //                         case TCP_CLOSE_WAIT:
        //                                 /**
        //                                  *  CLOSING STATE
        //                                  *      Remain in the CLOSING state.
        //                                  */
        //                         case TCP_CLOSING:
        //                                 /**
        //                                  *  LAST-ACK STATE
        //                                  *      Remain in the LAST-ACK state.
        //                                  */
        //                         case TCP_LAST_ACK:
        //                                 /**
        //                                  *  TIME-WAIT STATE
        //                                  *      Remain in the TIME-WAIT state.  Restart the 2 MSL
        //                                  * time-wait timeout.
        //                                  */
        //                         case TCP_TIME_WAIT:
        //                                 return;
        //                 }
        //                 DLOG(INFO) << "[TCP] [PROCESS 9] " << *in_tcb;
        //         }
        // }
};
}  // namespace mstack