#pragma once

#include <stdint.h>

// Define a standard macro to check the size of the data type
#define CheckSize(dataType, expectedSize)           \
    static_assert(sizeof(dataType) == expectedSize, \
                  "ERROR: " #dataType " size is not correct. (Should be " #expectedSize " bytes)")

#pragma pack(push, 8)

namespace ParkingInfrastructure
{
/// Stream source constants.
enum StreamSource_e : uint16_t
{
    /// Indicates the stream was sent by the Fusion PC.
    FUSION_PC = 40,
    /// Indicates the stream was sent by the parking infrastructure.
    INFRASTRUCTURE = 63
};

/// Messages related to automated parking feature enablement.
namespace Enablement
{
/// Common types for Enablement messages.
namespace Types
{
/// Vehicle status which describes the state of the vehicle for the TTM.
enum class VehicleStatus_e : uint8_t
{
    /// Vehicle is not requesting automated parking services.
    STANDBY = 0,
    /// Vehicle is ready to engage in automated parking.
    READY = 1,
    /// Vehicle is actively searching for a parking slot or parking.
    AUTO_DRIVING = 2,
    /// Vehicle has completed the parking maneuver.
    PARKED = 3,
    /// Vehicle is blocked by an obstacle that has interrupted the automated parking maneuver.
    BLOCKED = 4,
    /// Vehicle had to stop the automated parking maneuver.
    EMERGENCY_STOP = 5
};
} // namespace Types
/// Stream definitions
namespace Streams
{
/// Streams sent from the vehicle
namespace Vehicle
{
/// Description: This is the periodic message sent from the vehicle to the infrastructure indicating that infrastructure
/// based automated parking is in the appropriate state and is able to activate or to stay enabled.
///
/// Behavior Definition: Determined by the parking infrastructure.
namespace Heartbeat
{
/// Stream source number.
constexpr StreamSource_e STREAM_SOURCE = StreamSource_e::FUSION_PC;
/// Stream message number.
constexpr uint16_t STREAM_NUMBER = 143;
/// Stream version number.
constexpr uint16_t STREAM_VERSION = 1;
/// Stream payload definition.
struct Payload
{
    /// [ms] Timestamp from a GPS synchronized local clock source.
    uint64_t timestamp_ms;
    /// The ID of the vehicle.
    uint64_t vehicleId;
    /// The current vehicle status.
    Types::VehicleStatus_e vehicleStatus;
};
CheckSize(Payload, 24U);
} // namespace Heartbeat
} // namespace Vehicle

/// Streams sent from the infrastructure
namespace Infrastructure
{
/// Description: This is the periodic message sent from the infrastructure to the vehicle indicating that infrastructure
/// based automated parking is in the appropriate state and is able to activate or to stay enabled.
///
/// Behavior Definition: The vehicle will not respond to infrastructure based automated parking requests if it ceases to
/// receive periodic heartbeat messages from the infrastructure. This means that a disablement request will be sent to
/// the MABx.
namespace Heartbeat
{
/// Stream source number.
constexpr StreamSource_e STREAM_SOURCE = StreamSource_e::INFRASTRUCTURE;
/// Stream message number.
constexpr uint16_t STREAM_NUMBER = 5;
/// Stream version number.
constexpr uint16_t STREAM_VERSION = 1;
/// Stream payload definition.
struct Payload
{
    /// [ms] Timestamp from a GPS synchronized local clock source.
    uint64_t timestamp_ms;
    /// The ID of the vehicle.
    uint64_t vehicleId;
};
CheckSize(Payload, 16U);
} // namespace Heartbeat
} // namespace Infrastructure
} // namespace Streams
} // namespace Enablement

/// Localization message namespace.
namespace Localization
{
/// Common types for Localization messages.
namespace Types
{
/// Coordinate system types.
enum class CoordinateSystem_e : uint8_t
{
    WGS84 = 0,
    UTM   = 1
};

/// Coordinate frame type.
struct CoordinateFrame
{
    /// Specifies which coordinate system this coordinate frame is in.
    CoordinateSystem_e coordinateSystem;
    // Padding bytes
    uint8_t padding[3];
    /// Specifies the zone for the coordinate frame origin. The interpretation of this value depends on the coordinate
    /// system. IF WGS84: Unused IF UTM: UTM zone.
    uint32_t originZone;
};

/// The estimated pose of the vehicle. The vehicle coordinate frame origin is the center of the rear axle.
///
/// The orientation of the vehicle relative to the world is described by the rotation matrix @f$ R_v^w @f$, defined as
///
///   @f\[
///     R_v^w = R_z(\psi) R_y(\theta) R_x(\phi),
///   @f\]
///
/// where @f$ \psi @f$ is the yaw angle, @f$ \theta @f$ is the pitch angle, and @f$ \phi @f$ is the roll angle.
struct Pose
{
    /// X-Position. The interpretation of this value depends on the coordinate system.
    /// IF WGS84:   [deg]   Longitudinal position.
    /// IF UTM:     [m]     Easting coordinate.
    double x;
    /// Y-Position. The interpretation of this value depends on the coordinate system.
    /// IF WGS84:   [deg]   Lateral position.
    /// IF UTM:     [m]     Northing coordinate.
    double y;
    /// Z-Position. [m] Positive is up.
    double z_m;
    /// [rad] [-π, π] Right-handed rotation about the vehicle ISO frame x-axis.
    double roll_rad;
    /// [rad] [-π/2, π/2] Right-handed rotation about the ISO intermediate frame y-axis.
    double pitch_rad;
    /// [rad] [-π, π] Right-handed rotation about the world frame z-axis.
    double yaw_rad;
};

/// The uncertainty of the estimated pose.
struct PoseUncertainty
{
    /// [m²] Uncertainty in vehicle pose x estimate.
    double x;
    /// [m²] Uncertainty in vehicle pose y estimate.
    double y;
    /// [m²] Uncertainty in vehicle pose z estimate.
    double z_m2;
    /// [rad²] Uncertainty in vehicle pose roll estimate.
    double roll_rad2;
    /// [rad²] Uncertainty in vehicle pose pitch estimate.
    double pitch_rad2;
    /// [rad²] Uncertainty in vehicle pose yaw estimate.
    double yaw_rad2;
};
} // namespace Types

/// Stream definitions
namespace Streams
{
/// Streams sent by the vehicle
namespace Vehicle
{
/// Description: This is the host vehicle localization estimate sent from the vehicle to the infrastructure.
///
/// Behavior Definition: It is expected that the infrastructure will use this information for selecting future
/// waypoints and for sanity checking. This information should not be used to modify localization data sent by
/// the infrastructure back to the vehicle.
namespace Localization
{
/// Stream source number.
constexpr StreamSource_e STREAM_SOURCE = StreamSource_e::FUSION_PC;
/// Stream message number.
constexpr uint16_t STREAM_NUMBER = 140;
/// Stream version number.
constexpr uint16_t STREAM_VERSION = 1;
/// Stream payload definition.
struct Payload
{
    /// [ms] The timestamp corresponding to the localization estimate.
    uint64_t timestamp_ms;
    /// The coordinate frame of the localization update.
    Types::CoordinateFrame coordinateFrame;
    /// The mean estimate of the localization update.
    Types::Pose state;
    /// The uncertainty of the localization update.
    Types::PoseUncertainty uncertainty;
};
CheckSize(Payload, 112U);
} // namespace Localization
} // namespace Vehicle

/// Streams sent by the infrastructure
namespace Infrastructure
{
/// Description: This is the host vehicle localization estimate sent from the infrastructure to the vehicle. The
/// infrastructure uses sensing functionality such as LiDAR, camera, etc to estimate vehicle poses.
///
/// Behavior Definition: If the vehicle is receiving regular heartbeat messages, this infrastructure localization will
/// attempt to be fused into the vehicle's on board localization filter. It will effectively be treated as if it were
/// any other GNSS measurement, such as GPS.
namespace Localization
{
/// Stream source number.
constexpr StreamSource_e STREAM_SOURCE = StreamSource_e::INFRASTRUCTURE;
/// Stream message number.
constexpr uint16_t STREAM_NUMBER = 1;
/// Stream version number.
constexpr uint16_t STREAM_VERSION = 1;
/// Stream payload definition.
struct Payload
{
    /// [ms] The timestamp corresponding to the localization estimate.
    uint64_t timestamp_ms;
    /// The coordinate frame of the localization update.
    Types::CoordinateFrame coordinateFrame;
    /// The mean estimate of the localization update.
    Types::Pose state;
    /// The uncertainty of the localization update.
    Types::PoseUncertainty uncertainty;
};
CheckSize(Payload, 112U);
} // namespace Localization
} // namespace Infrastructure
} // namespace Streams
} // namespace Localization

/// Routing messages namespace.
namespace Routing
{
/// Common types for Routing messages.
namespace Types
{
/// TODO: Need more details on what this mode specifies.
enum class Mode_e : uint8_t
{
    STARTUP  = 0,
    DRIVE    = 1,
    PARK     = 2,
    STOP     = 3,
    SHUTDOWN = 4
};

/// Route request types.
enum class RequestType_e : uint8_t
{
    PARK   = 0,
    SUMMON = 1
};

/// Structure that defines a single routing waypoint.
struct WayPoint
{
    /// The index of the waypoint.
    int32_t index;
    // Padding bytes
    uint8_t padding[4];
    /// The x coordinate of the waypoint in the same coordinate frame as the localization message.
    double x;
    /// The y coordinate of the waypoint in the same coordinate frame as the localization message.
    double y;
    /// The z coordinate of the waypoint in the same coordinate frame as the localization message.
    double z_m;
    /// [1/m] Path curvature.
    double k;
    /// [m/s] Maximum allowed speed.
    double maxSpeed_mps;
    /// The width of drivable area to the right of the waypoint.
    double laneWidthRight_m;
    /// The width of the drivable area to the left of the waypoint.
    double laneWidthLeft_m;
};
} // namespace Types

/// Stream definition
namespace Streams
{
/// Streams sent by the vehicle
namespace Vehicle
{
/// Description: When the vehicle is ready to be routed to a parking space or to be routed to the back to the customer
/// for a summon, this routing request is sent to the infrastructure.
///
/// Behavior Definition: It is expected that the infrastructure will respond with a route message which contains
/// waypoints of where the vehicle should go to arrive at its destination.
namespace Request
{
/// Stream source number.
constexpr StreamSource_e STREAM_SOURCE = StreamSource_e::FUSION_PC;
/// Stream message number.
constexpr uint16_t STREAM_NUMBER = 142;
/// Stream version number.
constexpr uint16_t STREAM_VERSION = 1;
/// Stream payload definition.
struct Payload
{
    /// [ms] The timestamp corresponding to the routing message.
    uint64_t timestamp_ms;
    /// The type of route being requested.
    Types::RequestType_e requestType;
};
CheckSize(Payload, 16U);
} // namespace Request
} // namespace Vehicle

/// Streams sent by the infrastructure
namespace Infrastructure
{
/// Description: This is the route information that is sent to the vehicle after the vehicle requests a route.
///
/// Behavior Definition: If the vehicle is receiving regular heartbeat messages from the infrastructure and the MABx
/// is requesting routing information from the Fusion PC, this routing information will be repackaged into the existing
/// VaPA path follower interface and sent to the MABx for the local path planner to follow.
namespace Routing
{
/// Stream source number.
constexpr StreamSource_e STREAM_SOURCE = StreamSource_e::INFRASTRUCTURE;
/// Stream message number.
constexpr uint16_t STREAM_NUMBER = 11;
/// Stream version number.
constexpr uint16_t STREAM_VERSION = 1;
/// The maximum number of waypoints that can be included in each message.
constexpr uint16_t WAYPOINT_ARRAY_SIZE = 255U;
/// Stream payload definition.
struct Payload
{
    /// [ms] The timestamp corresponding to the routing message.
    uint64_t timestamp_ms;
    /// TODO: Need more information on this mode.
    Types::Mode_e mode;
    // Padding bytes
    uint8_t padding1;
    /// The number of valid waypoints in the @c waypoints array.
    uint16_t numberOfWaypoints;
    // Padding bytes
    uint8_t padding2[4];
    /// The waypoints that make up the route.
    Types::WayPoint waypoints[WAYPOINT_ARRAY_SIZE];
    /// The index of the waypoint corresponding to the destination.
    int32_t destinationWaypointIndex;
    // Padding bytes
    uint8_t padding3[4];
};
CheckSize(Payload, 16344U);
} // namespace Routing
} // namespace Infrastructure
} // namespace Streams
} // namespace Routing

/// Parking space availability namespace.
namespace ParkingSlotAvailability
{
/// Constants for parking slot availability messages.
namespace Constants
{
/// The maximum number of slots that information can be returned for in a single query.
constexpr size_t SLOT_INFORMATION_SIZE = 255;
} // namespace Constants

/// Common types for parking slot availability messages.
namespace Types
{
/// Enumeration for encoding parking slot availability status.
enum class ParkingSlotStatus_e : uint8_t
{
    /// Parking slot is available for use.
    AVAILABLE = 0,
    /// Parking slot is unavailable for use.
    OCCUPIED = 1,
    /// Parking slot status is unknown, should assume it cannot be used.
    UNKNOWN = 2
};

/// Enumeration that defines which slot corner point corresponds to which index. The corners are defined from the
/// perspective of an observer looking into the slot from the slot entrance. This same convention should be followed for
/// perpendicular, parallel and fish-bone slots.
enum SlotCorners_e : uint8_t
{
    FRONT_LEFT  = 0,
    REAR_LEFT   = 1,
    REAR_RIGHT  = 2,
    FRONT_RIGHT = 3,
    NUM_CORNERS = 4
};

/// Types associated with WGS84 GNSS.
namespace WGS84
{
/// Represents a WGS84 position.
struct Position_t
{
    /// Longitudinal coordinate of position [deg].
    double longitudinalCoordinate_deg;
    /// Lateral coordinate of position [deg].
    double lateralCoordinate_deg;
};

/// Represents a WGS84 pose.
struct Pose_t : public Position_t
{
    /// Pose heading [rad].
    /// Due north is 0 rad. Positive clock-wise rotation such that due East heading is π/2 and West heading is 3π/2.
    /// Heading defined in range [0 2π].
    double heading_rad;
};

/// Quadrilateral for WGS84 positions.
using Quadrilateral_t = Position_t[Types::NUM_CORNERS];
} // namespace WGS84

/// Defines the parking slot geometry for a single slot.
struct ParkingSlotGeometry_t
{
    /// The corner locations of the parking slot.
    WGS84::Quadrilateral_t parkingSlotQuad;
};
} // namespace Types

/// Stream definitions
namespace Streams
{
/// Streams sent by the vehicle
namespace Vehicle
{
/// Description: Request to the infrastructure for the location of the closest available parking slot.
///
/// Behavior Definition: If parking slot availability infrastructure is available, a response containing
/// the geometry of the closest available slot is expected. If a response is not received within an appropriate
/// time window, the drop-off route planning will default to a route that explores the entire parking lot.
namespace ClosestAvailableSlotRequest
{
/// Stream source number.
constexpr StreamSource_e STREAM_SOURCE = StreamSource_e::FUSION_PC;
/// Stream message number.
constexpr uint16_t STREAM_NUMBER = 141;
/// Stream version number.
constexpr uint16_t STREAM_VERSION = 1;
/// Stream payload definition.
struct Payload
{
    /// [ms] The timestamp corresponding to the closest available slot request message.
    uint64_t timestamp_ms;
    /// The pose for which the associated parking lot ID is being requested.
    Types::WGS84::Pose_t referencePose;
};
CheckSize(Payload, 32U);
} // namespace ClosestAvailableSlotRequest
} // namespace Vehicle

/// Streams sent by the infrastructure
namespace Infrastructure
{
/// Description: Contains the location of the closest available parking slot to the vehicle.
///
/// Behavior Definition: Currently it is assumed that parking slot availability information will only be used
/// if the vehicle is in a surface lot automated parking maneuver. If configured to request parking slot information,
/// a request will be made for parking slot information. If a response is received within an allowable amount of
/// time, the drop-off route will be planned from the vehicle's current pose to the location of the parking slot using
/// the on-board parking lot map.
namespace ClosestAvailableParkingSlotResponse
{
/// Stream source number.
constexpr StreamSource_e STREAM_SOURCE = StreamSource_e::INFRASTRUCTURE;
/// Stream message number.
constexpr uint16_t STREAM_NUMBER = 21;
/// Stream version number.
constexpr uint16_t STREAM_VERSION = 1;
/// Stream payload definition.
struct Payload
{
    /// [ms] The timestamp corresponding to the closest available slot information source.
    uint64_t timestamp_ms;
    /// The corner location of the closest parking slot.
    Types::ParkingSlotGeometry_t closestSlotGeometry;
};
CheckSize(Payload, 72U);
} // namespace ClosestAvailableParkingSlotResponse
} // namespace Infrastructure
} // namespace Streams
} // namespace ParkingSlotAvailability
} // namespace ParkingInfrastructure

#pragma pack(pop)
