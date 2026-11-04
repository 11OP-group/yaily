const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const exe = b.addExecutable(.{
        .name = "yaily", 
        .root_module = b.createModule(.{
            .target = target,
            .optimize = optimize,
            .link_libc = true,
        }),
    });

    exe.root_module.addIncludePath(b.path("include"));

    exe.root_module.addCSourceFiles(.{
        .files = &.{ 
            "src/main.c", 
            "src/utils.c", 
        },
        .flags = &.{ "-Wall", "-Wextra", "-std=c11" },
    });

    b.installArtifact(exe);


    const run_cmd = b.addRunArtifact(exe);
    run_cmd.step.dependOn(b.getInstallStep());
    const run_step = b.step("run", "Запустить приложение");
    run_step.dependOn(&run_cmd.step);
}
