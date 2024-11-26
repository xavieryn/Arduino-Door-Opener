"use client";

import { Button } from "@/components/ui/button";
import { useState } from "react";

function Bluetooth() {
  const [connected, isConnected] = useState(false);
  const connectToDevice = async () => {
    navigator.bluetooth.requestDevice({
      filters: [{ namePrefix: "Arduino R4" }, { services: ["Service ID"] }],
    });
    isConnected(true);
  };

  return (
    <div>
      {connected ? (
        <div>
          <Button onClick={connectToDevice}>Connect to Device</Button>
        </div>
      ) : (
        <div className="flex flex-col gap-2">
          Rest of info
          <Button onClick={() => isConnected(true)}>
            Set Connected to false
          </Button>
        </div>
      )}
    </div>
  );
}
export default Bluetooth;
