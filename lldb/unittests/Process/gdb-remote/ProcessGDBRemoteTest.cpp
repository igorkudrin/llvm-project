//===-- ProcessGDBRemoteTest.cpp ------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "Plugins/Process/gdb-remote/ProcessGDBRemote.h"
#include "GDBRemoteTestUtils.h"
#include "Plugins/Platform/MacOSX/PlatformMacOSX.h"
#include "Plugins/Platform/MacOSX/PlatformRemoteMacOSX.h"
#include "lldb/Core/Debugger.h"
#include "lldb/Host/ConnectionFileDescriptor.h"
#include "lldb/Host/FileSystem.h"
#include "lldb/Host/HostInfo.h"
#include "llvm/Testing/Support/Error.h"
#include "gtest/gtest.h"

using namespace lldb_private::process_gdb_remote;
using namespace lldb_private;
using namespace lldb;
typedef GDBRemoteCommunication::PacketResult PacketResult;

namespace {

class TestProcess : public ProcessGDBRemote {
public:
  TestProcess(TargetSP target_sp, ListenerSP listener_sp)
      : ProcessGDBRemote(target_sp, listener_sp) {}

  void SetConnection(std::unique_ptr<Connection> connection) {
    m_gdb_comm.SetConnection(std::move(connection));
  }
};

typedef std::shared_ptr<TestProcess> TestProcessSP;

class ProcessGDBRemoteTest : public GDBRemoteTest {
public:
  void SetUp() override {
    FileSystem::Initialize();
    HostInfo::Initialize();
    PlatformMacOSX::Initialize();

    ArchSpec arch("x86_64-apple-macosx-");

    Platform::SetHostPlatform(
        PlatformRemoteMacOSX::CreateInstance(true, &arch));
    debugger_sp = Debugger::CreateInstance();
    debugger_sp->GetTargetList().CreateTarget(
        *debugger_sp, "", arch, eLoadDependentsNo, platform_sp, target_sp);
    listener_sp = Listener::MakeListener("listener");
    process_sp = std::make_shared<TestProcess>(target_sp, listener_sp);

    llvm::Expected<Socket::Pair> pair = Socket::CreatePair();
    ASSERT_THAT_EXPECTED(pair, llvm::Succeeded());
    process_sp->SetConnection(
        std::make_unique<ConnectionFileDescriptor>(std::move(pair->first)));
    server.SetConnection(
        std::make_unique<ConnectionFileDescriptor>(std::move(pair->second)));
    // ASSERT_EQ(TestClient::eBroadcastBitRunPacketSent,
    //           listener_sp->StartListeningForEvents(
    //               &client, TestClient::eBroadcastBitRunPacketSent));
  }
  void TearDown() override {
    process_sp.reset();
    PlatformMacOSX::Terminate();
    HostInfo::Terminate();
    FileSystem::Terminate();
  }

protected:
  ArchSpec arch;
  DebuggerSP debugger_sp;
  ListenerSP listener_sp;
  PlatformSP platform_sp;
  TargetSP target_sp;
  TestProcessSP process_sp;
  MockServer server;

  //  TestClient client;
  //  MockDelegate delegate;
  //
  //  StateType SendCPacket(StringExtractorGDBRemote &response) {
  //    return client.SendContinuePacketAndWaitForResponse(delegate,
  //    LinuxSignals(),
  //                                                       "c", g_timeout,
  //                                                       response);
  //  }
};

} // end anonymous namespace

TEST_F(ProcessGDBRemoteTest, t) {
  ASSERT_EQ(PacketResult::Success, server.SendPacket("E00"));
  ASSERT_FALSE(process_sp->CanJIT());
  // StringExtractorGDBRemote response;
  //
  //// Continue. The inferior will stop with a signal.
  // ASSERT_EQ(PacketResult::Success, server.SendPacket("T01"));
  // ASSERT_EQ(eStateStopped, SendCPacket(response));
  // ASSERT_EQ("T01", response.GetStringRef());
  // ASSERT_EQ(PacketResult::Success, server.GetPacket(response));
  // ASSERT_EQ("c", response.GetStringRef());
  //
  //// Continue. The inferior will exit.
  // ASSERT_EQ(PacketResult::Success, server.SendPacket("W01"));
  // ASSERT_EQ(eStateExited, SendCPacket(response));
  // ASSERT_EQ("W01", response.GetStringRef());
  // ASSERT_EQ(PacketResult::Success, server.GetPacket(response));
  // ASSERT_EQ("c", response.GetStringRef());
  //
  //// Continue. The inferior will get killed.
  // ASSERT_EQ(PacketResult::Success, server.SendPacket("X01"));
  // ASSERT_EQ(eStateExited, SendCPacket(response));
  // ASSERT_EQ("X01", response.GetStringRef());
  // ASSERT_EQ(PacketResult::Success, server.GetPacket(response));
  // ASSERT_EQ("c", response.GetStringRef());
}
